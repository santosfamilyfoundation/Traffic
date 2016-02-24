

# get object id, first and last frame numbers, length and number of features
select OF.object_id, min(P.frame_number) as frame1, max(P.frame_number) as frame2, max(P.frame_number)-min(P.frame_number) as length, count(OF.trajectory_id) as nfeatures from positions P, objects_features OF where P.trajectory_id = OF.trajectory_id group by OF.object_id order by nfeatures asc;