%% Simulation Data
simu = simulationClass();               % Initialize Simulation Class
simu.simMechanicsFile = 'PAs.slx';    % Specify Simulink Model File
simu.mode = 'normal';                   % Specify Simulation Mode ('normal','accelerator','rapid-accelerator')
simu.explorer = 'on';                   % Turn SimMechanics Explorer (on/off)
simu.startTime = 0;                     % Simulation Start Time [s]
simu.rampTime = 20;                    % Wave Ramp Time [s]
simu.endTime = 100;                     % Simulation End Time [s]        
simu.solver = 'ode4';                   % simu.solver = 'ode4' for fixed step & simu.solver = 'ode45' for variable step 
simu.dt = 0.01;                          % Simulation Time-Step [s]
simu.domainSize = 100;
        
%% Wave Information
% % noWaveCIC, no waves with radiation CIC  
% waves = waveClass('noWaveCIC');       % Initialize Wave Class and Specify Type  

% Regular Waves 
waves = waveClass('regular');           % Initialize Wave Class and Specify Type                                 
waves.height = 2;                     % Wave Height [m]
waves.period = 5;                       % Wave Period [s]

% Irregular Waves using PM Spectrum with Directionality 
% waves = waveClass('irregular');         % Initialize Wave Class and Specify Type
% waves.height = 2.5;                     % Significant Wave Height [m]
% waves.period = 8;                       % Peak Period [s]
% waves.spectrumType = 'JS';              % Specify Spectrum Type
% waves.direction = [0,30,90];            % Wave Directionality [deg]
% waves.spread = [0.1,0.2,0.7];           % Wave Directional Spreading [%}

% % Irregular Waves with imported spectrum
% waves = waveClass('spectrumImport');      % Create the Wave Variable and Specify Type
% waves.spectrumFile = 'spectrumData.mat';  % Name of User-Defined Spectrum File [:,2] = [f, Sf]

% % Waves with imported wave elevation time-history  
% waves = waveClass('elevationImport');          % Create the Wave Variable and Specify Type
% waves.elevationFile = 'elevationData.mat';     % Name of User-Defined Time-Series File [:,2] = [time, eta]


%% Body Data
% cylinder
body(1) = bodyClass('multiple_cy.h5');      % Initialize bodyClass for cylinder
body(1).geometryFile = 'cylinder.stl';     % Geometry File
body(1).mass = 'equilibrium';                          % User-Defined mass [kg]
body(1).inertia = [548604 548604 117558];       % Moment of Inertia [kg-m^2]
body(1).nonlinearHydro=0;

body(2) = bodyClass('cy_dataset.h5');      % Initialize bodyClass for cylinder
body(2).geometryFile = 'cylinder.stl';     % Geometry File
body(2).mass = 'equilibrium';                          % User-Defined mass [kg]
body(2).inertia = [548604 548604 117558];       % Moment of Inertia [kg-m^2]
body(2).nonlinearHydro=0;

body(3) = bodyClass('cy_dataset.h5');      % Initialize bodyClass for cylinder
body(3).geometryFile = 'cylinder.stl';     % Geometry File
body(3).mass = 'equilibrium';                          % User-Defined mass [kg]
body(3).inertia = [548604 548604 117558];       % Moment of Inertia [kg-m^2]
body(3).nonlinearHydro=0;

body(4) = bodyClass('cy_dataset.h5');      % Initialize bodyClass for cylinder
body(4).geometryFile = 'cylinder.stl';     % Geometry File
body(4).mass = 'equilibrium';                          % User-Defined mass [kg]
body(4).inertia = [548604 548604 117558];       % Moment of Inertia [kg-m^2]
body(4).nonlinearHydro=0;


%% PTO and Constraint Parameters
% Fixed
constraint(1)= constraintClass('Constraint1');  % Initialize ConstraintClass for Constraint1
constraint(1).location = [0 0 0];             % Constraint Location [m]

constraint(2)= constraintClass('Constraint2');  % Initialize ConstraintClass for Constraint1
constraint(2).location = [20 0 0];             % Constraint Location [m]

constraint(3)= constraintClass('Constraint3');  % Initialize ConstraintClass for Constraint1
constraint(3).location = [20 20 0];             % Constraint Location [m]

constraint(4)= constraintClass('Constraint4');  % Initialize ConstraintClass for Constraint1
constraint(4).location = [0 20 0];             % Constraint Location [m]

% PTO
pto(1) = ptoClass('PTO1');                      % Initialize ptoClass for PTO1
pto(1).stiffness = 0;                           % PTO Stiffness Coeff [Nm/rad]
pto(1).damping = 56000;                         % PTO Damping Coeff [Nsm/rad]
pto(1).location = [0 0 0];                   % PTO Location [m]

pto(2) = ptoClass('PTO2');                      % Initialize ptoClass for PTO1
pto(2).stiffness = 0;                           % PTO Stiffness Coeff [Nm/rad]
pto(2).damping = 56000;                         % PTO Damping Coeff [Nsm/rad]
pto(2).location = [20 0 0];                   % PTO Location [m]

pto(3) = ptoClass('PTO3');                      % Initialize ptoClass for PTO1
pto(3).stiffness = 0;                           % PTO Stiffness Coeff [Nm/rad]
pto(3).damping = 56000;                         % PTO Damping Coeff [Nsm/rad]
pto(3).location = [20 20 0];                   % PTO Location [m]

pto(4) = ptoClass('PTO4');                      % Initialize ptoClass for PTO1
pto(4).stiffness = 0;                           % PTO Stiffness Coeff [Nm/rad]
pto(4).damping = 56000;                         % PTO Damping Coeff [Nsm/rad]
pto(4).location = [0 20 0];                   % PTO Location [m]
