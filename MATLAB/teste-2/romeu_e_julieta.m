clear all; close all; clc;

fs = 48000; % frequencia do .wav

DRC_Ratio = 5.0; 
DRC_Knee_Width = 0; % dB
DRC_Attack_Time = 0.05; % s
DRC_Release_Time = 0.4; % s
DRC_Make_up_Gain = 0; % dB

AGC_Step_Size = 0.5;
AGC_Averging_Window_Length = 0.4*fs; % 100 ms para uma frequencia = fs
AGC_Maximum_Power_Gain = 10;

STOI_results_file = fopen("STOI_results.txt", 'w');
fprintf(STOI_results_file,"    DRC-18    DRC-24    AGC-18    AGC-24\n");

for i=1:20

    input_path = "samples\" + num2str(i) + ".wav";
    input_track = audioread(input_path);

    simTim = length(input_track)/fs; % s
    out = sim("cravo_e_canela.slx");
    
    % BO do comprimento do array
    min_length = min( [length(input_track), length(out.DRC_18), length(out.DRC_24), length(out.AGC_18), length(out.AGC_24)]);
    input_track = input_track(1:min_length);
    DRC_18 = out.DRC_18(1:min_length);
    DRC_24 = out.DRC_24(1:min_length);
    AGC_18 = out.AGC_18(1:min_length);
    AGC_24 = out.AGC_24(1:min_length);

    audiowrite("output/" + i + "_DRC-18.wav",DRC_18,fs);
    audiowrite("output/" + i + "_DRC-24.wav",DRC_24,fs);
    audiowrite("output/" + i + "_AGC-18.wav",AGC_18,fs);
    audiowrite("output/" + i + "_AGC-24.wav",AGC_24,fs);
    
    STOI_DRC_18 = 100*stoi(DRC_18,input_track,fs);
    STOI_DRC_24 = 100*stoi(DRC_24,input_track,fs);
    STOI_AGC_18 = 100*stoi(AGC_18,input_track,fs);
    STOI_AGC_24 = 100*stoi(AGC_24,input_track,fs);

    fprintf(STOI_results_file,"%.2d  %.2f     %.2f     %.2f     %.2f\n",i, STOI_DRC_18, STOI_DRC_24, STOI_AGC_18, STOI_AGC_24);

end

fclose(STOI_results_file);