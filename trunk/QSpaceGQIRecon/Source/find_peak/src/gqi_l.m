function Q2ODF=gqi_l(odf_vertices,b_table,mean_diffusion_distance_ratio);
% Q2ODF10 = gqi_l(odf_vertices',b_table,1.0);
% Q2ODF11 = gqi_l(odf_vertices,b_table,1.1);
% Q2ODF12 = gqi_l(odf_vertices,b_table,1.2);
l_values = sqrt(b_table(:,1)*0.01506);
b_vector = b_table(:,2:4).*repmat(l_values,1,3);
Q2ODF = sinc(b_vector*odf_vertices*mean_diffusion_distance_ratio/pi);

% show odf S being the signals.
% odf_points = odf_vertices.*repmat(Q2ODF12'*S-min(Q2ODF12'*S), [1 3])';
% scatter3(odf_points(1,:),odf_points(2,:),odf_points(3,:));
end


