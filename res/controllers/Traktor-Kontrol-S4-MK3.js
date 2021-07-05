class S4MK3 {
    constructor() {
        this.potMax = 2**12;
    }
    incomingData(data) {
        const reportId = data[0];
        // slice off the reportId
        const buffer = data.buffer.slice(1);
        if (reportId === 2) {
            const view = new Uint16Array(buffer, 0, buffer.byteLength/2);

            engine.setParameter("[Master]", "crossfader", view[0]/this.potMax);

            engine.setParameter("[Channel1]", "volume", view[1]/this.potMax);
            engine.setParameter("[Channel2]", "volume", view[2]/this.potMax);
            engine.setParameter("[Channel3]", "volume", view[3]/this.potMax);
            engine.setParameter("[Channel4]", "volume", view[4]/this.potMax);

            engine.setParameter("[Channel2]", "rate", view[5]/this.potMax);
            engine.setParameter("[Channel1]", "rate", view[6]/this.potMax);

            engine.setParameter("[Channel3]", "pregain", view[7]/this.potMax);
            engine.setParameter("[Channel1]", "pregain", view[8]/this.potMax);
            engine.setParameter("[Channel2]", "pregain", view[9]/this.potMax);
            engine.setParameter("[Channel4]", "pregain", view[10]/this.potMax);

            // These control the controller's audio interface in hardware, so do not map them.
            // engine.setParameter('[Master]', 'gain', view[11]/this.potMax);
            // engine.setParameter('[Master]', 'booth_gain', view[12]/this.potMax);
            // engine.setParameter('[Master]', 'headMix', view[13]/this.potMax);
            // engine.setParameter('[Master]', 'headVolume', view[14]/this.potMax);

            engine.setParameter("[EffectRack1_EffectUnit1]", "mix", view[15]/this.potMax);
            engine.setParameter("[EffectRack1_EffectUnit1_Effect1]", "meta", view[16]/this.potMax);
            engine.setParameter("[EffectRack1_EffectUnit1_Effect2]", "meta", view[17]/this.potMax);
            engine.setParameter("[EffectRack1_EffectUnit1_Effect3]", "meta", view[18]/this.potMax);

            engine.setParameter("[EqualizerRack1_[Channel3]_Effect1]", "parameter3", view[19]/this.potMax);
            engine.setParameter("[EqualizerRack1_[Channel3]_Effect1]", "parameter2", view[20]/this.potMax);
            engine.setParameter("[EqualizerRack1_[Channel3]_Effect1]", "parameter1", view[21]/this.potMax);

            engine.setParameter("[EqualizerRack1_[Channel1]_Effect1]", "parameter3", view[22]/this.potMax);
            engine.setParameter("[EqualizerRack1_[Channel1]_Effect1]", "parameter2", view[23]/this.potMax);
            engine.setParameter("[EqualizerRack1_[Channel1]_Effect1]", "parameter1", view[24]/this.potMax);

            engine.setParameter("[EqualizerRack1_[Channel2]_Effect1]", "parameter3", view[25]/this.potMax);
            engine.setParameter("[EqualizerRack1_[Channel2]_Effect1]", "parameter2", view[26]/this.potMax);
            engine.setParameter("[EqualizerRack1_[Channel2]_Effect1]", "parameter1", view[27]/this.potMax);

            engine.setParameter("[EqualizerRack1_[Channel4]_Effect1]", "parameter3", view[28]/this.potMax);
            engine.setParameter("[EqualizerRack1_[Channel4]_Effect1]", "parameter2", view[29]/this.potMax);
            engine.setParameter("[EqualizerRack1_[Channel4]_Effect1]", "parameter1", view[30]/this.potMax);

            engine.setParameter("[QuickEffectRack1_[Channel3]]", "super1", view[31]/this.potMax);
            engine.setParameter("[QuickEffectRack1_[Channel1]]", "super1", view[32]/this.potMax);
            engine.setParameter("[QuickEffectRack1_[Channel2]]", "super1", view[33]/this.potMax);
            engine.setParameter("[QuickEffectRack1_[Channel4]]", "super1", view[34]/this.potMax);

            engine.setParameter("[EffectRack1_EffectUnit2]", "mix", view[35]/this.potMax);
            engine.setParameter("[EffectRack1_EffectUnit2_Effect1]", "meta", view[36]/this.potMax);
            engine.setParameter("[EffectRack1_EffectUnit2_Effect2]", "meta", view[37]/this.potMax);
            engine.setParameter("[EffectRack1_EffectUnit2_Effect3]", "meta", view[38]/this.potMax);
        }
    }
    init() {
    }
    shutdown() {
    }
}

var TraktorS4MK3 = new S4MK3();
// ugly hack to get eslint to not complain TraktorS4MK3 is unused
TraktorS4MK3.foo = null;
