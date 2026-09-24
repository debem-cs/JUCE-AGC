input_track = audioread("samples\speechSample-2.wav");
AGC_script = audioread("AGC_script.wav");
AGC_JUCE = audioread("AGC_JUCE.wav");
fs = 48000;

%%%%%%%%%%%%%%%%%%%%%%% BO do comprimento do array %%%%%%%%%%%%%%%%%%%%%%%%
min_length = min([length(input_track), length(AGC_script), length(AGC_JUCE)]);
input_track = input_track(1:min_length);
AGC_script = AGC_script(1:min_length);
AGC_JUCE = AGC_JUCE(1:min_length);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

simTim = length(input_track)/fs;

STOI_metric_AGC_script = stoi(AGC_script,input_track,fs)
STOI_metric_AGC_JUCE = stoi(AGC_JUCE,input_track,fs)