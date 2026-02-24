declare filename "SteganoDecoder.dsp";
declare name "SteganoDecoder";
import("stdfaust.lib");

f_carrier = 18000;

process(stego_in) = signal_final
with {
    filtrage_entree = stego_in : fi.bandpass(4, f_carrier-1000, f_carrier+1000);

    demodulated = filtrage_entree * os.osc(f_carrier);

    recovered_voice = demodulated : fi.lowpass(4, 1800) * 150;

    voice_eq = recovered_voice : fi.peak_eq(6, 850, 1.0); // +6dB à 850Hz

    voice_comp = voice_eq : co.compressor_mono(4, -15, 0.01, 0.1);

    voice_gate = voice_comp : ef.gate_mono(-54, 0.01, 0.01, 0.1);

    signal_final = voice_gate : aa.hardclip2;
};