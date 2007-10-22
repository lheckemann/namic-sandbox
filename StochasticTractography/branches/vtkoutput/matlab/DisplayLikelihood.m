function DisplayLikelihood( likelihoodfilename, mode )
%mode=1 no scaling
%mode=2 log scaling 
    
    load('vectors2562.mat');
    
    likelihoodvalues = load(likelihoodfilename);
    s_vectors=[atan2(vectors(2,:),vectors(1,:)); acos(vectors(3,:))];
    
    likelihood = [s_vectors; likelihoodvalues'];
    
    tri = delaunay(likelihood(1,:),likelihood(2,:));
    
    s_tri=delaunay(s_vectors(1,:),s_vectors(2,:));
    figure;
    for i=1:length(vectors)
        scaled_vectors(:,i) = likelihoodvalues(i)*vectors(:,i);
    end
    %trisurf(s_tri,scaled_vectors(1,:),scaled_vectors(2,:),scaled_vectors(3,:));
    %figure;
    if(mode==1)
        trisurf(tri, likelihood(1,:),likelihood(2,:), likelihood(3,:));
    else
        trisurf(tri, likelihood(1,:),likelihood(2,:), -log(likelihood(3,:)));
    end
    
    view([180 90]);
    
    figure;
    [gridX,gridY]=meshgrid([0:0.01:pi],[-pi:0.01:pi]);
    interplikelihood=interp2(likelihood(1,:),likelihood(2,:),likelihood(3,:),gridX,gridY,'nearest');