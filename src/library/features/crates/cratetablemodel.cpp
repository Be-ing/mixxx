#include "library/features/crates/cratetablemodel.h"

#include "library/dao/trackschema.h"
#include "library/trackcollection.h"
#include "mixer/playermanager.h"
#include "util/db/fwdsqlquery.h"

#include <QtDebug>

CrateTableModel::CrateTableModel(QObject* pParent,
                                 TrackCollection* pTrackCollection)
        : BaseSqlTableModel(pParent, pTrackCollection,
                            "mixxx.db.model.crate"),
          m_pCrates(pTrackCollection->crates()) {
}

CrateTableModel::~CrateTableModel() {
}

void CrateTableModel::selectCrate(const Crate& crate) {
    //qDebug() << "CrateTableModel::setCrate()" << crateId;
    m_selectedCrate = crate.getId();

    QString recursion = "";
    QString queryPart = "";
    if (m_pCrates->isRecursionEnabled() &&
        m_pCrates->hierarchy().hasChildren(crate.getId())) {
        // we need a different table for recursive view so we set a different name
        // that table is only needed when the crate has children, cause then subcrate tracks
        // wont's show if we create some subcrates later, until the db connection is restarted
        recursion = "recursive";
        queryPart = QString("%1 IN (%2) OR ").arg(
          CRATETRACKSTABLE_CRATEID,
          m_pCrates->hierarchy().formatQueryForChildCrateIds(crate));
    }
    queryPart = queryPart + QString("%1 IN (%2)").arg(
      CRATETRACKSTABLE_CRATEID,
      crate.getId().toString());

    QString tableName = QString("crate_%1_%2").arg(m_selectedCrate.toString(),
                                                   recursion);
    if (crate.getId() == m_selectedCrate &&
        getTableName() == tableName) {
        qDebug() << "Already focused on crate " << crate.getId();
        return;
    }

    QStringList columns;
    columns << LIBRARYTABLE_ID
            << "'' AS " + LIBRARYTABLE_PREVIEW
            // For sorting the cover art column we give LIBRARYTABLE_COVERART
            // the same value as the cover hash.
            << LIBRARYTABLE_COVERART_HASH + " AS " + LIBRARYTABLE_COVERART;
    // We hide files that have been explicitly deleted in the library
    // (mixxx_deleted = 0) from the view.
    // They are kept in the database, because we treat crate membership as a
    // track property, which persist over a hide / unhide cycle.
    QString queryString = QString("CREATE TEMPORARY VIEW IF NOT EXISTS %1 AS "
                                  "SELECT DISTINCT %2 FROM %3 "
                                  "JOIN %4 ON %5 = %6 "
                                  "WHERE %7=0 AND "
                                  "%8")
                          .arg(tableName,
                               columns.join(","),
                               LIBRARY_TABLE,
                               CRATE_TRACKS_TABLE,
                               CRATETRACKSTABLE_TRACKID,
                               LIBRARYTABLE_ID,
                               LIBRARYTABLE_MIXXXDELETED,
                               queryPart);

    FwdSqlQuery(m_database, queryString).execPrepared();

    columns[0] = LIBRARYTABLE_ID;
    columns[1] = LIBRARYTABLE_PREVIEW;
    columns[2] = LIBRARYTABLE_COVERART;
    setTable(tableName, LIBRARYTABLE_ID, columns,
             m_pTrackCollection->getTrackSource());

    setDefaultSort(fieldIndex("artist"), Qt::AscendingOrder);
}

bool CrateTableModel::addTrack(const QModelIndex& index, QString location) {
    Q_UNUSED(index);

    // This will only succeed if the file actually exist.
    QFileInfo fileInfo(location);
    if (!fileInfo.exists()) {
        qDebug() << "CrateTableModel::addTrack:"
                << "File"
                << location
                << "not found";
        return false;
    }

    TrackDAO& trackDao = m_pTrackCollection->getTrackDAO();
    // If a track is dropped but it isn't in the library, then add it because
    // the user probably dropped a file from outside Mixxx into this crate.
    // If the track is already contained in the library it will not insert
    // a duplicate. It also handles unremoving logic if the track has been
    // removed from the library recently and re-adds it.
    const TrackPointer pTrack(trackDao.addSingleTrack(fileInfo, true));
    if (!pTrack) {
        qDebug() << "CrateTableModel::addTrack:"
                << "Failed to add track"
                << location
                << "to library";
        return false;
    }

    QList<TrackId> trackIds;
    trackIds.append(pTrack->getId());
    if (m_pCrates->addTracksToCrate(m_selectedCrate, trackIds)) {
        // TODO(rryan) just add the track dont select
        select();
        return true;
    } else {
        qDebug() << "CrateTableModel::addTrack:"
                << "Failed to add track"
                << location
                << "to crate"
                << m_selectedCrate;
        return false;
    }
}

TrackModel::CapabilitiesFlags CrateTableModel::getCapabilities() const {
    CapabilitiesFlags caps =  TRACKMODELCAPS_NONE
            | TRACKMODELCAPS_RECEIVEDROPS
            | TRACKMODELCAPS_ADDTOPLAYLIST
            | TRACKMODELCAPS_ADDTOCRATE
            | TRACKMODELCAPS_ADDTOAUTODJ
            | TRACKMODELCAPS_IMPORTMETADATA
            | TRACKMODELCAPS_LOADTODECK
            | TRACKMODELCAPS_LOADTOSAMPLER
            | TRACKMODELCAPS_LOADTOPREVIEWDECK
            | TRACKMODELCAPS_REMOVE
            | TRACKMODELCAPS_MANIPULATEBEATS
            | TRACKMODELCAPS_CLEAR_BEATS
            | TRACKMODELCAPS_RESETPLAYED;
    if (m_selectedCrate.isValid()) {
        Crate crate;
        if (m_pCrates->storage().readCrateById(m_selectedCrate, &crate)) {
            if (crate.isLocked()) {
                caps |= TRACKMODELCAPS_LOCKED;
            }
        } else {
            qWarning() << "Failed to read create" << m_selectedCrate;
        }
    }
    return caps;
}

bool CrateTableModel::isColumnInternal(int column) {
    return column == fieldIndex(ColumnCache::COLUMN_LIBRARYTABLE_ID) ||
            column == fieldIndex(ColumnCache::COLUMN_LIBRARYTABLE_PLAYED) ||
            column == fieldIndex(ColumnCache::COLUMN_LIBRARYTABLE_MIXXXDELETED) ||
            column == fieldIndex(ColumnCache::COLUMN_LIBRARYTABLE_BPM_LOCK) ||
            column == fieldIndex(ColumnCache::COLUMN_LIBRARYTABLE_KEY_ID)||
            column == fieldIndex(ColumnCache::COLUMN_TRACKLOCATIONSTABLE_FSDELETED) ||
            (PlayerManager::numPreviewDecks() == 0 &&
             column == fieldIndex(ColumnCache::COLUMN_LIBRARYTABLE_PREVIEW)) ||
            column == fieldIndex(ColumnCache::COLUMN_LIBRARYTABLE_COVERART_SOURCE) ||
            column == fieldIndex(ColumnCache::COLUMN_LIBRARYTABLE_COVERART_TYPE) ||
            column == fieldIndex(ColumnCache::COLUMN_LIBRARYTABLE_COVERART_LOCATION) ||
            column == fieldIndex(ColumnCache::COLUMN_LIBRARYTABLE_COVERART_HASH);;
}

int CrateTableModel::addTracks(const QModelIndex& index,
                               const QList<QString>& locations) {
    Q_UNUSED(index);
    // If a track is dropped but it isn't in the library, then add it because
    // the user probably dropped a file from outside Mixxx into this crate.
    QList<QFileInfo> fileInfoList;
    foreach(QString fileLocation, locations) {
        QFileInfo fileInfo(fileLocation);
        if (fileInfo.exists()) {
            fileInfoList.append(fileInfo);
        }
    }

    QList<TrackId> trackIds(m_pTrackCollection->getTrackDAO().addMultipleTracks(fileInfoList, true));
    if (m_pCrates->addTracksToCrate(m_selectedCrate, trackIds)) {
        select();
        return trackIds.size();
    } else {
        qWarning() << "CrateTableModel::addTracks could not add"
                 << locations.size()
                 << "tracks to crate" << m_selectedCrate;
        return 0;
    }
}

void CrateTableModel::removeTracks(const QModelIndexList& indices) {
    VERIFY_OR_DEBUG_ASSERT(m_selectedCrate.isValid()) {
        return;
    }
    if (indices.empty()) {
        return;
    }

    Crate crate;
    if (!m_pCrates->storage().readCrateById(m_selectedCrate, &crate)) {
        qWarning() << "Failed to read create" << m_selectedCrate;
        return;
    }

    VERIFY_OR_DEBUG_ASSERT(!crate.isLocked()) {
        return;
    }

    QList<TrackId> trackIds;
    trackIds.reserve(indices.size());
    for (const QModelIndex& index: indices) {
        trackIds.append(getTrackId(index));
    }
    if (m_pCrates->removeTracksFromCrate(crate.getId(), trackIds)) {
        select();
    } else {
        qWarning() << "Failed to remove tracks from crate" << crate;
    }
}