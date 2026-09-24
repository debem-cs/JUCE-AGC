input_track_path = "samples\speechSample-7.wav";
fs = 48000; % frequencia do .wav

input_track = audioread(input_track_path);

simTim = length(input_track)/fs;
out = sim("AGC_script.slx");

AGC = out.AGC;
AGC_script = out.AGC_script;

%%%%%%%%%%%%%%%%%%%%%%% BO do comprimento do array %%%%%%%%%%%%%%%%%%%%%%%%
min_length = min([length(input_track), length(AGC), length(AGC_script)]);
input_track = input_track(1:min_length);
AGC = AGC(1:min_length);
AGC_script = AGC_script(1:min_length);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
STOI_metric_AGC = stoi(AGC,input_track,fs)
STOI_metric_AGC_script = stoi(AGC_script,input_track,fs)

audiowrite('AGC.wav', AGC, fs);
audiowrite('AGC_script.wav', AGC_script, fs);