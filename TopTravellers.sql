# Write your MySQL query statement below
SELECT Users.name, IFNULL(SUM(Rides.distance), 0) as travelled_distance
FROM Users
LEFT JOIN Rides ON Users.id = Rides.user_id
GROUP BY  Rides.user_id 
ORDER BY travelled_distance
DESC, Users.name;
