clear
clc
close all

ncdisp("cy_dataset.nc");
added_mass=ncread("cy_dataset.nc","added_mass");
radiation_damping=ncread("cy_dataset.nc","radiation_damping");
omega=ncread("cy_dataset.nc","omega");
M=ncread("cy_dataset.nc","inertia_matrix");
hydrostatic_stiff=ncread("cy_dataset.nc","hydrostatic_stiffness");
A=squeeze(added_mass(3,3,:)); %Added mass matrix
B=squeeze(radiation_damping(3,3,:));
K=hydrostatic_stiff(3,3); %Heave hydrostatic stiffness

R_1=zeros(100,1);
X_1=zeros(100,1);
b_pto=zeros(100,1);
omega_index=zeros(15,1);
zeta=zeros(100,1); %percentage of critical damping
y=zeros(100,300);
x=1:300;
y0=1;
omega_n=1.2209; %natural frequency


for i=1:100

zeta(i,1)=(B(i,1)/(2*sqrt(K*(M(3,3)+A(i,1)))));
X_1(i)=1i*omega(i)*(M(3,3)+added_mass(3,3,i))+(hydrostatic_stiff(3,3)/1i);

for j=1:300
y(i,j)=y0*exp(-omega_n*zeta(i)*j)*cos(sqrt(1-(zeta(i))^2)*omega_n*j);       %free decay motion from t=1 to t=300s
end

end

y_a=y(13,:); %natural frequency index=13 
TF=islocalmax(y_a); %find index of x_k
x_k=y_a(TF); %x_k is the amplitude of the kth oscillation cycle
dimension=size(x_k);
number_x_k=dimension(2);
eqn_y=zeros(number_x_k-2,1);

for i=2:number_x_k-1
eqn_y(i-1)=(1/2/pi)*(log(x_k(i-1)/x_k(i+1)));
end
x_k1=zeros(number_x_k-2,1);
for i=1:number_x_k-2
    x_k1(i)=x_k(i+1);
end

c=polyfit(x_k1,eqn_y,1);
B2=c(1)*(M(3,3)+A(13))*3*pi/4;

for i=1:100
    R_1(i)=radiation_damping(3,3,i)+B2;                                       %R(w)=b_a(w)+R_f
    b_pto(i)=((R_1(i))^2+(imag(X_1(i)))^2)^(1/2);
end


% figure()
% plot(x,y(13,:),x(TF),y_a(TF),"r*")
% xlabel("time")
% ylabel("displacement")
% 
% b_pto(13)