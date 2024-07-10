clear 
clc
close all

for k=1:2
    analyze_loads(k)
end



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
    end
end
