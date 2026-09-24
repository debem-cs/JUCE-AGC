clear all; close all; clc;

input_track = "samples\speechSample-2b.wav";
noise_track = "samples\1000.wav";
original_track = audioread("samples\speechSample-2.wav");
fs = 48000; % frequencia do .wav

noiseVolume = 0;
inputVolume = 1;

Limiter_Threshold = -10; % dB

DRC_Threshold = -18; % dB
DRC_Ratio = 5.0; 
DRC_Knee_Width = 0; % dB
DRC_Attack_Time = 0.05; % s
DRC_Release_Time = 0.4; % s
DRC_Make_up_Gain = 0; % dB

AGC_Step_Size = 0.5;
AGC_Target = -18; % dB
AGC_Averging_Window_Length = 0.4*fs; % 100 ms para uma frequencia = fs
AGC_Maximum_Power_Gain = 10;

simTim = 6*60+29; % s
out = sim("cravoEcanela.slx");
compressed_DRC = out.DRC;
compressed_AGC = out.AGC;

%%%%%%%%%%%%%%%%%%%%%%% BO do comprimento do array %%%%%%%%%%%%%%%%%%%%%%%%
min_length = min(length(compressed_DRC), length(original_track));
compressed_DRC = compressed_DRC(1:min_length);
original_track = original_track(1:min_length);

min_length = min(length(compressed_AGC), length(original_track));
compressed_AGC = compressed_AGC(1:min_length);
original_track = original_track(1:min_length);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
STOI_metric_DRC = stoi(compressed_DRC,original_track,fs);
STOI_metric_AGC = stoi(compressed_AGC,original_track,fs);

audiowrite('DRC.wav', compressed_DRC, fs);
audiowrite('AGC.wav', compressed_AGC, fs);