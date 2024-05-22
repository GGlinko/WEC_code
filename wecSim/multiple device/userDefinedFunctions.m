% filename = sprintf('savedData%03d.mat', imcr);
% 
% mcr.Avgpower(imcr) = mean(output.ptos.powerInternalMechanics(2000:end,3));
% mcr.CPTO(imcr)  = pto(1).damping;
% 
% save (filename, 'mcr','output','waves');
% 
% 
% if imcr == length(mcr.cases)
%     H = mcr.cases(:,1);
%     T = mcr.cases(:,2);
%     bpto=mcr.CPTO(1,:);
%     P = abs(mcr.Avgpower)/1000;
%     T_number=sum(H == 1);
%     H_number=sum(T==1);
% 
%     % Damping = 28000
%     figure
%     matrix=transpose(reshape(P,T_number,H_number));
%     imagesc(matrix);                                   % Create a colored plot of the matrix values
%     colormap default
%     caxis([min(matrix(:)) max(matrix(:))])
%     textStrings = num2str(matrix(:),'%0.1f');          % Create strings from the matrix values
%     textStrings = strtrim(cellstr(textStrings));    % Remove any space padding
%     [x,y] = meshgrid(1:15,1:9);                          % Create x and y coordinates for the strings
%     hStrings = text(x(:),y(:),textStrings(:),'HorizontalAlignment','center','FontSize',6);    % Plot the strings
%     midValue = mean(get(gca,'CLim'));               % Get the middle value of the color range
%     textColors = repmat(matrix(:) < midValue,1,3);     % Choose white or black for the text color
%     set(hStrings,{'Color'},num2cell(textColors,2)); % Change the text colors
%     colorbar
%     set(gca,'XTick',1:T_number,'YTick',1:H_number)
%     xlabel('Wave Period [s]')
%     ylabel('Significant Wave Height [m]')
%     ylabel(colorbar,'Power [kW]')
%     title(['Power Matrix for Damping =' num2str(bpto(1)) '[N/m/s] (WecSim)'])
% 
% 
% 
% end
%Plot waves
waves.plotElevation(simu.rampTime);
try 
    waves.plotSpectrum();
catch
end

t = output.bodies.time; % Time vector

N = 4; % Number of floaters
figure()

heave = cell(1, N); % Cell array to store heave response for each floater

% Extract heave response for each floater
for i = 1:N
    heave{i} = output.bodies(i).position(:, 3);
end

% Plot heave response for all floaters
hold on;
for i = 1:N
    plot(t, heave{i});
end

% Create legend entries
legendEntries = arrayfun(@(x) ['floater_', num2str(x)], 1:N, 'UniformOutput', false);

legend(legendEntries{:})
title('Heave response of the 6 floaters')
xlabel('Time [s]')
ylabel('Heave response [m]')
hold off;
