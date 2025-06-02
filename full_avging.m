clear all; clc; close all;
%%
test1 = cell2mat(struct2cell(load('Test1.mat')));
t_1 = test1(1:7200003,1);
dat_1 = test1(1:7200003,2);

test2 = cell2mat(struct2cell(load('Test2.mat')));
t_2 = test2(1:7200003,1);
dat_2 = test2(1:7200003,2);

test3 = cell2mat(struct2cell(load('Test3.mat')));
t_3 = test3(1:7200003,1);
dat_3 = test3(1:7200003,2);

test4 = cell2mat(struct2cell(load('Test4.mat')));
t_4 = test4(1:7200003,1);
dat_4 = test4(1:7200003,2);

test5 = cell2mat(struct2cell(load('Test5.mat')));
t_5 = test5(1:7200003,1);
dat_5 = test5(1:7200003,2);

test6 = cell2mat(struct2cell(load('Test6.mat')));
t_6 = test6(1:7200003,1);
dat_6 = test6(1:7200003,2);

test8 = cell2mat(struct2cell(load('Test8.mat')));
t_8 = test8(1:7200003,1);
dat_8 = test8(1:7200003,2);

test9 = cell2mat(struct2cell(load('Test9.mat')));
t_9 = test9(1:7200003,1);
dat_9 = test9(1:7200003,2);

test10 = cell2mat(struct2cell(load('Test10.mat')));
t_10 = test10(1:7200003,1);
dat_10 = test10(1:7200003,2);

test11 = cell2mat(struct2cell(load('Test11.mat')));
t_11 = test11(1:7200003,1);
dat_11 = test11(1:7200003,2);

%%
theoretical = readmatrix("Jon_theo_15rpm.csv");
dat_theory = theoretical(:,7);
t_theory = theoretical(:,1)*10/60;


%%
overall_avg = zeros(length(dat_11),1);

for i = 1:length(dat_11)
    all_vec = [dat_5(i) dat_6(i)...
        dat_8(i) dat_9(i) dat_10(i) dat_11(i)];
    overall_avg(i) = mean(all_vec);

end

%%
g_eff_1 = mean(dat_5(3599998:end,1));
g_eff_2 = mean(dat_6(3599998:end,1));
g_eff_3 = mean(dat_8(3599998:end,1));
g_eff_4 = mean(dat_9(3599998:end,1));
g_eff_5 = mean(dat_10(3599998:end,1));
g_eff_6 = mean(dat_11(3599998:end,1));

g_eff_vec = [g_eff_1, g_eff_2, g_eff_3, g_eff_4, g_eff_5, g_eff_6];

g_std = std(g_eff_vec);


g_eff_overall = mean(overall_avg(3599998:end,1));
g_eff_theory = mean(dat_theory(36001:72001));
g_error = 100*(g_eff_overall - g_eff_theory)/g_eff_theory
%%
figure(1)
plot(t_1, overall_avg, 'LineWidth', 1.5,'Color','k'); hold on;
plot(t_theory, dat_theory, 'LineWidth', 1.5,'Color', 'r', 'LineStyle','--')
xlabel('Time (min)')
ylabel('Effective Gravity (g)')
%title('Average Perceived Gravity for 6 Tests Compared With Theoretical Simulation')
%subtitle('2 Hour Tests Run at 15 rpm')
legend(sprintf('Average Gravity (%f g)',g_eff_overall), sprintf('Theoretical Gravity (%f g)',g_eff_theory),'Location','northeast')
xlim([0 120])

figure(2)
plot(t_5, dat_5, 'LineWidth', .75); hold on;
plot(t_6, dat_6, 'LineWidth', .75)
plot(t_8,dat_8, 'LineWidth', .75)
plot(t_9,dat_9, 'LineWidth', .75)
plot(t_10,dat_10, 'LineWidth', .75)
plot(t_11,dat_11, 'LineWidth', .75)
xlabel('Time (min)')
ylabel('Effective Gravity (g)')
%title('Perceived Gravity for 6 Tests')
%subtitle('2 Hour Tests Run at 15 rpm')
legend(sprintf('Test 1 (%f g)',g_eff_1), sprintf('Test 2 (%f g)',g_eff_2),...
    sprintf('Test 3 (%f g)',g_eff_3), sprintf('Test 4 (%f g)',g_eff_4), ...
    sprintf('Test 5 (%f g)',g_eff_5), sprintf('Test 6 (%f g)',g_eff_6),'Location', 'best')
xlim([0 120])

figure(3)
plot(t_5, dat_5, 'LineWidth', .75); hold on;
plot(t_6, dat_6, 'LineWidth', .75)
plot(t_8,dat_8, 'LineWidth', .75)
plot(t_9,dat_9, 'LineWidth', .75)
plot(t_10,dat_10, 'LineWidth', .75)
plot(t_11,dat_11, 'LineWidth', .75)
xlabel('Time (min)')
ylabel('Effective Gravity (g)')
%title('Perceived Gravity for 6 Tests')
%subtitle('2 Hour Tests Run at 15 rpm')
legend(sprintf('Test 1 (%f g)',g_eff_1), sprintf('Test 2 (%f g)',g_eff_2),...
    sprintf('Test 3 (%f g)',g_eff_3), sprintf('Test 4 (%f g)',g_eff_4), ...
    sprintf('Test 5 (%f g)',g_eff_5), sprintf('Test 6 (%f g)',g_eff_6), 'Location', 'best')
xlim([60 120])