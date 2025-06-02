clc; clear;

test = readmatrix(input('file name: ', 's'));

x_vec = test(:,2);
y_vec = test(:,3);
z_vec = test(:,4);
whole_vec = test(:,2:4);

x_mean = zeros(length(x_vec),1);
y_mean = zeros(length(x_vec),1);
z_mean = zeros(length(x_vec),1);
overall = zeros(length(x_vec),1);

x_mean(1) = x_vec(1);
y_mean(1) = y_vec(1);
z_mean(1) = z_vec(1);
overall(1) = norm([x_mean(1),y_mean(1), z_mean(1)]);

for m = 2:length(z_mean)
    x_mean(m) = ((x_mean(m-1)*(m-1))+x_vec(m))/m;
    y_mean(m) = ((y_mean(m-1)*(m-1))+y_vec(m))/m;
    z_mean(m) = ((z_mean(m-1)*(m-1))+z_vec(m))/m;
    overall(m) = norm([x_mean(m), y_mean(m), z_mean(m)]);
        
end

time = linspace(0,(length(x_vec)*.001)/60,length(overall));


output_mat  = [time' overall];




