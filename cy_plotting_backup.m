clear 
clc
close all
format long

ncdisp("cy_rao.nc")
rao_real=ncread("cy_rao.nc","real"); % rao exported from capytaine
rao_imag=ncread("cy_rao.nc","imag");
rao=rao_real+rao_imag*1i;
s=rao(3,:);% heave response amplitude operator

ncdisp("cy_dataset.nc");
added_mass=ncread("cy_dataset.nc","added_mass");
radiation_damping=ncread("cy_dataset.nc","radiation_damping");
omega=ncread("cy_dataset.nc","omega");
M=ncread("cy_dataset.nc","inertia_matrix");
hydrostatic_stiff=ncread("cy_dataset.nc","hydrostatic_stiffness");
F_diff=ncread("cy_dataset.nc","diffraction_force");
F_froude=ncread("cy_dataset.nc","Froude_Krylov_force");
F_d_heave=zeros(100,1);
F_f_heave=zeros(100,1);
M_total=zeros(100,1);
C=zeros(100,1);
% merge real and imaginary part of diffraction force and Froude Krylov,
% merge mass matrix and added mass matrix
% setup C_ij 
for i=1:100
    F_d_heave(i)=F_diff(3,1,i,1)+1i*F_diff(3,1,i,2);
    F_f_heave(i)=F_froude(3,1,i,1)+1i*F_froude(3,1,i,2);
    M_total(i)=M(3,3)+added_mass(3,3,i);
    C(i)=-(omega(i))^2*(M_total(i))+1i*omega(i)*radiation_damping(3,3,i)+hydrostatic_stiff(3,3);
end
F_excitation=F_d_heave+F_f_heave;
% calculate RAO
RAO=zeros(100,1);
for i=1:100
    RAO(i)=F_excitation(i)./C(i);
end


K=hydrostatic_stiff(3,3); %Heave hydrostatic stiffness
A=squeeze(added_mass(3,3,:)); %Added mass matrix
B_pto=28000; %Ns/m
value=zeros(100,1);

T_sign=sort(2*pi./omega);             %averaged zero-crossing period
H_sign=1:9;            
z_sign=H_sign/2;                         %significant amplitude
power=zeros(9,15);

for i=1:100
    value(i)=(omega(i))^2-(K/(A(i)+M(3,3)));
end
%plot the (omega,value) to find the intersection=natrural frequency
omega_n=omega(13); %natural frequency=1.22
z_a=zeros(9,15);
omega_index=zeros(15,1);
T_2=1:15;
omega_2=2*pi./T_2;

% calculate RAO include b_pto
C_b=zeros(100,1);
for i=1:100
    C_b(i)=-(omega(i))^2*(M_total(i))+1i*(omega(i)*radiation_damping(3,3,i)+B_pto)+hydrostatic_stiff(3,3);
end
RAO_b=zeros(100,1);
for i=1:100
    RAO_b(i)=F_excitation(i)./C_b(i);
end

%find the index of nearest omega to omega_2
for i=1:15
     [~,omega_index(i)]=min(abs(omega-omega_2(i)));
end
for i=1:9
    for j=1:15
        z_a(i,j)=z_sign(i)*abs(RAO_b(omega_index(j)));    
    end
end

% calculate power
for i=1:9
    for j=1:15
    power(i,j)=0.5*(omega_2(j))^2*B_pto*(z_a(i,j))^2/1000; %kW
    end
end


figure(1)
plot(omega,squeeze(added_mass(3,3,:)))
xlabel("wave frequency(rad/s)")
ylabel("added_mass")
title("Heave added mass")
figure(2)
plot(omega,squeeze(radiation_damping(3,3,:)))
xlabel("wave frequency(rad/s)")
ylabel("radiation damping")
title ("Heave radiation damping")



figure(3)
plot(omega,value)

figure(4)
plot(omega,abs(rao(3,:)))
xlabel("wave frequency(rad/s)")
ylabel("Heave RAO(-)")

%Compare figure 4 with figure 5
figure(5)
plot(omega,abs(RAO))

figure(6)
plot(omega,abs(RAO_b))

figure(7)
imagesc(power);
colormap default
caxis([min(power(:)) max(power(:))])
textStrings = num2str(power(:),'%0.1f');          % Create strings from the matrix values
textStrings = strtrim(cellstr(textStrings));    % Remove any space padding
[x,y] = meshgrid(1:15,1:9);                          % Create x and y coordinates for the strings
hStrings = text(x(:),y(:),textStrings(:),'HorizontalAlignment','center','FontSize',6);    % Plot the strings
midValue = mean(get(gca,'CLim'));               % Get the middle value of the color range
textColors = repmat(power(:) < midValue,1,3);     % Choose white or black for the text color
set(hStrings,{'Color'},num2cell(textColors,2)); % Change the text colors
colorbar
set(gca,'XTick',1:15,'YTick',1:9)
xlabel('Wave Period [s]')
ylabel('Significant Wave Height [m]')
ylabel(colorbar,'Power(kW)')
title(['Power Matrix for Damping = ' num2str(B_pto(end)) ' [N/m/s]'])

