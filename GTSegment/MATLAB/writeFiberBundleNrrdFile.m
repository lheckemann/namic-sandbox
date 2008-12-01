function writeFiberBundleNrrdFile( output )
% function writeFiberBundleNrrdFile( output )
% Write the fiber bundle to a nrrd file.
% Input Parameters:
%   output is a struct containing all of the output information
% Output Parameters:
%   n/a

fprintf('Writing the fiber bundle to a nrrd file... (writeFiberBundleNrrdFile.m)\n');

% Change the tract region labels to a blue label for slicer2
tractRegionSmooth = nrrdLoadWithMetadata( output.ioVars.roiNhdrFilePath ); % used as a reference data for the header info
tractRegionSmooth.data = uint8( zeros( size(tractRegionSmooth.data) ) );
tractRegionSmooth.data( output.cropfun(1)+1:output.cropfun(2)+1, ...
                        output.cropfun(3)+1:output.cropfun(4)+1, ...
                        output.cropfun(5)+1:output.cropfun(6)+1 ) = ...
    uint8( output.tractRegionSmooth.*17 );

% Call the save function for the smoothed data
nrrdSaveWithMetadata( output.ioVars.fbOutputFilePath, tractRegionSmooth );
