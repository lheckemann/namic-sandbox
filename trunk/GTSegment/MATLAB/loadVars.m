function [ioVars,processVars] = loadVars( ioVars )
% function [ioVars,processVars] = loadVars( ioVars )
% This file contains all of the user input parameters and returns
% them to the program as two structs.
% Input Parameters:
%   ioVars is a struct containing initially the user-specific vars
% Output Parameters:
%   ioVars is a struct containing all of the I/O variables
%   processVars is a struct containing all of the process variables

fprintf('Loading the runtime variables... (loadVars.m)\n');

% Add in the I/O variables
ioVars.tensorsNhdrFilePath = strcat( ioVars.outputDirPath, ...
                                     ioVars.commonOutputFilenamePrefix, ...
                                     'tensors.nhdr' );
ioVars.faNhdrFilePath = strcat( ioVars.outputDirPath, ...
                                ioVars.commonOutputFilenamePrefix, ...
                                'fa.nhdr' );
ioVars.confidenceThreshold = 0.5; % threshold for the tend anvol - Marek
ioVars.noisyThreshold = 10; % threshold the original DWI values
ioVars.ccThreshold = 2; % threshold for the trace

% Add in the process variables
processVars.testMode = 0; % a boolean for test mode
if( strcmp(ioVars.initialTensorsNhdrFilePath,'') ),
  processVars.estimateTensors = 1;
else,
  processVars.estimateTensors = 0;
end
processVars.ODFEstimKParam = 8; % a scalaing parameter of Yogesh ODF estimation
processVars.backTractRelTol = 1e-3; % error tolerance for back tracting
processVars.backTractAbsTol = 1e-6; % absolute tolerance for back tracting
processVars.maxStep = 5; % max time step for back tracting
processVars.sweepingConvergenceParam = 1000; % when to stop the fast sweeping with the ODF
processVars.backTractingConvergenceParam = 8;  % when to stop the back tracting
processVars.tractSmoothingKernelSize = 10; % size of averaging neighborhood
processVars.priorThreshold = 0.75; % threshold for initial tract points
