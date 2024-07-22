%% This is a function rewrite from 'relation_analysis.m' and 'four_bodies_square.m'
clear 
clc
close all

% Select spacing index and put it into the function

for k=2:6
analyze_loads(k)
end


% for k=1:6
%     analyze_power(k)
% end

function analyze_loads(k)
   

    % Mapping from index k to spacing distances
    spacings = [8, 16, 24, 32, 40, 48];
    spacing_distance = spacings(k);

    % File name
    file_name = sprintf("array_design_%d.nc", k);

    % Display NetCDF file content
    ncdisp(file_name);

    % Read data from NetCDF file
    added_mass = ncread(file_name, "added_mass");
    radiation_damping = ncread(file_name, "radiation_damping");
    omega = ncread(file_name, "omega");
    M = ncread(file_name, "inertia_matrix");
    hydrostatic_stiff = ncread(file_name, "hydrostatic_stiffness");
    F_diff = ncread(file_name, "diffraction_force");
    F_froude = ncread(file_name, "Froude_Krylov_force");

    % Initialize variables
    num_devices = 4;
    num_omega = length(omega);
    M_total = zeros(4, 4, num_omega);
    F_diff_combined = zeros(num_devices, 3, num_omega);
    F_froude_combined = zeros(num_devices, 3, num_omega);

    % Combine forces and total inertia matrix
    for i = 1:num_omega
        for j = 1:num_devices
            for m = 1:3
                F_diff_combined(j, m, i) = F_diff(j, m, i, 1) + 1i * F_diff(j, m, i, 2);
                F_froude_combined(j, m, i) = F_froude(j, m, i, 1) + 1i * F_froude(j, m, i, 2);
            end
        end
        M_total(:, :, i) = M + added_mass(:, :, i);
    end

    % Calculate excitation force
    F_excitation = abs(F_froude_combined + F_diff_combined) / 1000;

    % Plotting parameters
    colors = {'-or', '-xg', '-+b'};
    angles = [0, 45, 90];

    % Loop through each device and create plots
    for device_index = 1:num_devices
        legend_labels = cell(1, 3);

        % Plot device load
        figure('Name', sprintf('Device %d Load (Spacing = %d m)', device_index, spacing_distance), 'NumberTitle', 'off', 'Position', [100, 100, 1200, 600]);

        subplot(1, 2, 1);
        hold on;
        for angle_index = 1:3
            plot(omega, squeeze(F_excitation(device_index, angle_index, :)), colors{angle_index});
        end
        xlabel("Wave Frequency (rad/s)");
        ylabel("Load (kN)");
        legend_labels = {
            ['Wave angle = 0', char(176)],
            ['Wave angle = 45', char(176)],
            ['Wave angle = 90', char(176)]
        };
        legend(legend_labels);
        title(sprintf('Device %d Load (Spacing = %d m)', device_index, spacing_distance));
        hold off;

        % Linear fit and plot
        subplot(1, 2, 2);
        hold on;
        for angle_index = 1:3
            % Extract data
            data = squeeze(F_excitation(device_index, angle_index, :));

            % Perform linear fit
            p = polyfit(omega, data, 1);  % p(1) is the slope, p(2) is the intercept
            fit_line = polyval(p, omega);
            plot(omega, fit_line, colors{angle_index}(2));

            % Create legend entry with linear fit equation
            legend_labels{angle_index} = sprintf('Wave angle = %d%c, y = %.2fx + %.2f', angles(angle_index), char(176), p(1), p(2));
        end

        % Add labels and legend
        xlabel("Wave Frequency (rad/s)");
        ylabel("Load (kN)");
        legend(legend_labels);
        title(sprintf('Device %d Load Linear Fit (Spacing = %d m)', device_index, spacing_distance));
        hold off;
        % saveas(gcf,sprintf('D:/桌面/Figures/Device_%d_Load_spacing_%d.png', device_index, spacing_distance))
    end
end

%% Power function
function analyze_power(k)
    % Mapping from index k to spacing distances
    spacings = [8, 16, 24, 32, 40, 48];
    spacing_distance = spacings(k);

    % File name
    file_name = sprintf("array_design_%d.nc", k);

    % Display NetCDF file content
    ncdisp(file_name);
    
    % Read data from NetCDF file
    added_mass = ncread(file_name, "added_mass");
    radiation_damping = ncread(file_name, "radiation_damping");
    omega = ncread(file_name, "omega");
    M = ncread(file_name, "inertia_matrix");
    hydrostatic_stiff = ncread(file_name, "hydrostatic_stiffness");
    F_diff = ncread(file_name, "diffraction_force");
    F_froude = ncread(file_name, "Froude_Krylov_force");
    
    % Initialize variables
    num_devices = 4;
    num_direction = 3;
    num_omega = length(omega);
    M_total = zeros(4, 4, num_omega);
    F_diff_combined = zeros(num_devices,  num_direction, num_omega);
    F_froude_combined = zeros(num_devices,  num_direction, num_omega);
    B_pto=29000; %Ns/m
    C=zeros(4, 4, num_direction, num_omega);
    C_inverse=zeros(4, 4, num_direction, num_omega);
    RAO=zeros(num_devices, num_direction, num_omega);

     % Combine forces and total inertia matrix
    for i = 1:num_omega
        M_total(:, :, i) = M + added_mass(:, :, i);
        for j = 1:num_devices
            for m = 1:num_direction
                F_diff_combined(j, m, i) = F_diff(j, m, i, 1) + 1i * F_diff(j, m, i, 2);
                F_froude_combined(j, m, i) = F_froude(j, m, i, 1) + 1i * F_froude(j, m, i, 2);
                C(:, :, m, i)=-(omega(i))^2*(M_total(:, :, i))+1i*omega(i)*(radiation_damping(:, :, i)+B_pto)+hydrostatic_stiff(:, :);
                C_inverse(:, :, m, i)=inv(C(:, :, m, i));
            end
        end 
    end

    % Calculate excitation force
    F_excitation = abs(F_froude_combined + F_diff_combined); %[N]
    
    % calculate RAO
    for i=1:num_omega
        for m=1:num_direction
            RAO(:, m, i)=C_inverse(:, :, m, i)*F_excitation(:, m, i);  
        end
    end

    z_a=zeros(num_devices, num_direction, num_omega); %heave amplitude
    H_sign=2;   %significant wave height          
    z_sign=H_sign/2;                         %significant amplitude
    power=zeros(num_devices, num_direction, num_omega);

    % calculate response heave motion
    for i=1:num_omega
        for j=1:num_devices
            for m=1:num_direction
                z_a(j,m,i)=z_sign*abs(RAO(j, m, i));  
            end
        end
    end

    for i=1:num_omega
        for j=1:num_devices
            for m=1:num_direction
                power(j, m, i)=0.5*(omega(i))^2*B_pto*(z_a(j, m, i))^2/1000; %kW
            end
        end
    end
    power_total=squeeze(power(1, :, :)+power(2, :, :)+power(3, :, :)+power(4, :, :));

    % Plotting parameters
    colors = {'-or', '-xg', '-+b'};
    angles = [0, 45, 90];

   
    legend_labels = cell(1, 3);

    % Plot total power
    figure('Name', sprintf('Power at H_s=2 meter (Spacing = %d m)', spacing_distance), 'NumberTitle', 'off', 'Position', [100, 100, 1200, 600]);

    subplot(1, 2, 1);
    hold on;
    for angle_index = 1:3
        plot(omega, power_total(angle_index, :), colors{angle_index});
    end
    xlabel("Wave Frequency (rad/s)");
    ylabel("Power (kW)");
    legend_labels = {
        ['Wave angle = 0', char(176)],
        ['Wave angle = 45', char(176)],
        ['Wave angle = 90', char(176)]
    };
    legend(legend_labels,'Location','southwest');
    title(sprintf('Power at H_s=2 meter (Spacing = %d m)', spacing_distance));
    hold off;

    % Cubic fit and plot
    subplot(1, 2, 2);
    hold on;
    for angle_index = 1:3
        % Extract data
        data = squeeze(power_total(angle_index, :));
    
        % Perform cubic fit
        p = polyfit(omega, data, 3);  % Cubic fit
        fit_line = polyval(p, omega);
        plot(omega, fit_line, colors{angle_index}(2));
    
        % Create legend entry with cubic fit equation
        legend_labels{angle_index} = sprintf('Wave angle = %d%c, y = %.2fx^3 + %.2fx^2 + %.2fx + %.2f', angles(angle_index), char(176), p(1), p(2), p(3), p(4));
    end
    
    % Add labels and legend
    xlabel("Wave Frequency (rad/s)");
    ylabel("Power (kW)");
    legend(legend_labels,'Location','southwest','FontSize',6.5);
    title(sprintf('Power Cubic Fit (Spacing = %d m)', spacing_distance));
    hold off;

    % saveas(gcf, sprintf('D:/桌面/Figures/Power_at_spacing_%dm.png', spacing_distance))

end
