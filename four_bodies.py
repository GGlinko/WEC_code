import numpy as np
import pandas as pd
import random
from numpy import infty
import xarray as xr
import matplotlib
from numpy import reshape
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
from mpl_toolkits.mplot3d.axes3d import Axes3D
import capytaine as cpt
from capytaine.io.xarray import separate_complex_values
from capytaine.io.legacy import export_hydrostatics 
from   capytaine.io.xarray import merge_complex_values
from capytaine.post_pro import rao
from capytaine.meshes.symmetric import build_regular_array_of_meshes
from   capytaine.bem.airy_waves import airy_waves_free_surface_elevation
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import sys
import math
import seaborn as sns
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.ensemble import RandomForestRegressor
from sklearn.svm import SVR
from sklearn.neural_network import MLPRegressor
from sklearn.metrics import mean_squared_error, r2_score

# Import logging module and set to 'debug' level
import logging
logging.basicConfig(level=logging.INFO)

# Import mesh file
mesh=cpt.load_mesh("D:\Rhino\cylinder.STL", file_format="stl",name="cylinder")
mesh1=cpt.load_mesh("D:\Rhino\cylinder.STL", file_format="stl",name="cylinder")
# radius=2, height=10


# Flow control
full_analysis=False
single_analysis=True
# full_analysis = True
# single_analysis = False


cylinder1=cpt.FloatingBody(mesh=mesh,name='body1')
cylinder1.center_of_mass=np.array([0,0,0])
cylinder1.rotation_center=np.array([0,0,0])
cylinder1.add_translation_dof(name="Heave")
cylinder1.keep_immersed_part()
cylinder1.inertia_matrix = cylinder1.compute_rigid_body_inertia()
cylinder1.hydrostatic_stiffness = cylinder1.compute_hydrostatic_stiffness(rho=1000)

def create_square_array(size):
    """Create a square array design with the given size."""
    return np.array([[0, 0], [0, size], [size, 0], [size, size]])

# Sizes for different square arrays
sizes = [8, 16, 24, 32, 40, 48]

# List to store the arrays
array_designs = []

# Loop to create and store the arrays
for size in sizes:
    array_designs.append(create_square_array(size))

all_buoys_list = []

# Loop to apply each array design to the buoys
for array_design in array_designs:
    all_buoys = cylinder1.assemble_arbitrary_array(array_design)
    all_buoys_list.append(all_buoys)

# Check the array shape 
all_buoys_list[1].show()

# Create 4 cylinders in different array
array_design_square=np.array([[0,0],[0,40],[40,0],[40,40]])
# array_design_line=np.array([[0,60],[0,20],[0,-20],[0,-60]])
# array_design_align_line=np.array([[60,0],[20,0],[-20,0],[-60,0]])
# array_design_diamond=np.array([[0,0],[0,40],[34,20],[-34,20]],dtype=int)
# array_design_parallelogram=np.array([[0,0],[20,34],[40,0],[60,34]],dtype=int)
# array_design_triangle=np.array([[0,30],[40,30],[20,18],[20,-5]])

# Set up WEC farm with different array
# all_buoys=cylinder1.assemble_arbitrary_array(array_design_square)
# bodies=cylinder1.assemble_regular_array(40,(2,2))

# Make the other three device fixed to plot the radiation field in single analysis
case1_bodies=cylinder1.assemble_regular_array(40,(2,2))
case1_bodies.keep_only_dofs(dofs=['0_0__Heave'])
case2_bodies=cylinder1.assemble_regular_array(40,(2,2))
case2_bodies.keep_only_dofs(dofs=['1_0__Heave'])
case3_bodies=cylinder1.assemble_regular_array(40,(2,2))
case3_bodies.keep_only_dofs(dofs=['0_1__Heave'])
case4_bodies=cylinder1.assemble_regular_array(40,(2,2))
case4_bodies.keep_only_dofs(dofs=['1_1__Heave'])

if full_analysis:
    # activate when exporting ncfile to do regression analysis in matlab
    omega_range=np.linspace(0.5,2,16) 
    # activate when running machine learning
    # omega_range=np.linspace(0.01,10,100)
    direction_range=[0, math.pi/4, math.pi/2]
    datasets={}
    F_excitation_all = []
    
    for i, all_buoys in enumerate(all_buoys_list, start=1):
        radiation_problems = [cpt.RadiationProblem(body=all_buoys, radiating_dof=dof, omega=omega)
                    for dof in all_buoys.dofs
                    for omega in omega_range]
            

        diffraction_problems = [cpt.DiffractionProblem(body=all_buoys, wave_direction=direction, omega=omega)
                    for omega in omega_range
                    for direction in direction_range]
            
        
        # Solve the Capytaine problem
        solver = cpt.BEMSolver()
        radiation_results = solver.solve_all(radiation_problems)
        diffraction_results=solver.solve_all(diffraction_problems)
        dataset = cpt.assemble_dataset(radiation_results+diffraction_results)
        rao_result = rao(dataset, wave_direction=0)
        datasets[f'array_design_{i}'] = dataset

    # solver.compute_potential(all_buoys, radiation_results) # may raise problem

    # Export output data
        filename = f"D:/capytaine_data/array_design_{i}.nc"
        separate_complex_values(dataset).to_netcdf(filename,
                    encoding={'radiating_dof': {'dtype': 'U'},
                                'influenced_dof': {'dtype': 'U'}})

        F_combined = np.real(dataset.diffraction_force) + np.real(dataset.Froude_Krylov_force)
        F_excitation = np.abs(F_combined)
        
        # Append the combined excitation force values to the list
        F_excitation_all.append(F_excitation.data)


    for device_index in range(4):
        data = []

        for array_idx, F_excitation in enumerate(F_excitation_all):
            for i in range(F_excitation.shape[0]):  # Frequency dimension
                for j in range(F_excitation.shape[1]):  # Wave angle dimension
                    data.append([omega_range[i], direction_range[j], sizes[array_idx], F_excitation[i, j, device_index]])


        df = pd.DataFrame(data, columns=['Frequency', 'Wave_Angle', 'Array_Design', 'Load'])

        # Splitting the data
        X = df[['Frequency', 'Wave_Angle', 'Array_Design']]
        y = df['Load']

        X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

        # Standardizing the features
        scaler = StandardScaler()
        X_train = scaler.fit_transform(X_train)
        X_test = scaler.transform(X_test)

        # Random Forest Regressor
        rf = RandomForestRegressor(n_estimators=100, random_state=42)
        rf.fit(X_train, y_train)
        y_pred_rf = rf.predict(X_test)

        # Evaluate the model
        mse_rf = mean_squared_error(y_test, y_pred_rf)
        r2_rf = r2_score(y_test, y_pred_rf)
        print(f"Random Forest - MSE: {mse_rf}, R^2: {r2_rf}")

        # Support Vector Regressor
        svr = SVR(kernel='rbf')
        svr.fit(X_train, y_train)
        y_pred_svr = svr.predict(X_test)

        # Evaluate the model
        mse_svr = mean_squared_error(y_test, y_pred_svr)
        r2_svr = r2_score(y_test, y_pred_svr)
        print(f"SVR - MSE: {mse_svr}, R^2: {r2_svr}")

        # Neural Network Regressor
        mlp = MLPRegressor(hidden_layer_sizes=(100,), max_iter=1000, random_state=42)
        mlp.fit(X_train, y_train)
        y_pred_mlp = mlp.predict(X_test)

        # Evaluate the model
        mse_mlp = mean_squared_error(y_test, y_pred_mlp)
        r2_mlp = r2_score(y_test, y_pred_mlp)
        print(f"Neural Network - MSE: {mse_mlp}, R^2: {r2_mlp}")

        # Plotting the actual vs predicted loads for each device
        plt.figure(device_index + 1,figsize=(12,6))
        plt.suptitle(f'Device {device_index + 1} Load Machine Learning Analysis')

        plt.subplot(1, 3, 1)
        plt.scatter(y_test, y_pred_rf, color='blue', alpha=0.5)
        plt.xlabel('Actual Loads')
        plt.ylabel('Predicted Loads')
        plt.title('Random Forest Regressor')
        plt.plot([y_test.min(), y_test.max()], [y_test.min(), y_test.max()], 'k--', lw=2)

        plt.subplot(1, 3, 2)
        plt.scatter(y_test, y_pred_svr, color='green', alpha=0.5)
        plt.xlabel('Actual Loads')
        plt.ylabel('Predicted Loads')
        plt.title('Support Vector Regressor')
        plt.plot([y_test.min(), y_test.max()], [y_test.min(), y_test.max()], 'k--', lw=2)

        plt.subplot(1, 3, 3)
        plt.scatter(y_test, y_pred_mlp, color='red', alpha=0.5)
        plt.xlabel('Actual Loads')
        plt.ylabel('Predicted Loads')
        plt.title('Neural Network Regressor')
        plt.plot([y_test.min(), y_test.max()], [y_test.min(), y_test.max()], 'k--', lw=2)

        plt.tight_layout(rect=[0, 0, 1, 0.96])
        plt.show(block=False)
    plt.show()

    # Define the inputs and test the model
    frequency = 2.5
    wave_angle = np.pi / 8
    array_spacing = 60

    # Create a DataFrame for the input
    input_data = pd.DataFrame([[frequency, wave_angle, array_spacing]], columns=['Frequency', 'Wave_Angle', 'Array_Design'])

    # Standardize the input data
    input_data_standardized = scaler.transform(input_data)

    # Make predictions using the trained models
    pred_rf = rf.predict(input_data_standardized)
    pred_svr = svr.predict(input_data_standardized)
    pred_mlp = mlp.predict(input_data_standardized)

    print(f"Random Forest Prediction: {pred_rf[0]}")
    print(f"Support Vector Regressor Prediction: {pred_svr[0]}")
    print(f"Neural Network Regressor Prediction: {pred_mlp[0]}")








if single_analysis:

    all_buoys=cylinder1.assemble_arbitrary_array(array_design_square)
    omega_range=2

    radiation_problems = [cpt.RadiationProblem(omega=omega_range, body=all_buoys, water_depth=infty,radiating_dof=dof) for dof in all_buoys.dofs]
    
    diffraction_problem = cpt.DiffractionProblem(omega=omega_range, body=all_buoys, water_depth=infty)

    problems = [cpt.RadiationProblem(body=all_buoys, radiating_dof=dof, omega=omega_range)
            for dof in all_buoys.dofs]
    problems += [cpt.DiffractionProblem(body=all_buoys, wave_direction=0.0, omega=omega_range)]

    # Solve the Capytaine problem
    solver = cpt.BEMSolver()
    radiation_result = solver.solve_all(radiation_problems)
    diffraction_result=solver.solve(diffraction_problem)
    results=solver.solve_all(problems)
   
    
    
    combined_radiation_pressure = np.zeros_like(np.real(radiation_result[0].pressure))
    for i in range(4):
        combined_radiation_pressure += np.real(radiation_result[i].pressure)
    
    all_buoys.show_matplotlib(
        color_field=np.real(combined_radiation_pressure+diffraction_result.pressure),
        cmap=plt.get_cmap("viridis"),  # Colormap
        cbar_label="Pressure(Pa)"
        )
    

    #Postprocessing
    dataset = cpt.assemble_dataset(radiation_result+[diffraction_result])
    rao_result = rao(dataset, wave_direction=0)

   # Free surface mesh
    xmin=-30
    xmax=70
    ymin=-30
    ymax=70
    nx=100
    ny=100
    x  = np.linspace(xmin, xmax, nx, endpoint=True)
    y  = np.linspace(ymin, ymax, ny, endpoint=True)
    points = np.meshgrid(x, y, np.linspace(-100.0, 0.0, 100))
    grid=np.meshgrid(x,y)
    fs = cpt.FreeSurface(x_range=(xmin, xmax), y_range=(ymin, ymax), nx=nx, ny=ny, name = 'free_surface')

    rao_at_omega_2=[0.05144,0.05152,0.02512,0.02488]
    # Compute individual wave patterns:
    incoming_waves_elevation = airy_waves_free_surface_elevation(grid,diffraction_result) 
    diffraction_elevation = solver.compute_free_surface_elevation(grid,diffraction_result)
    radiation_dof = {rad.radiating_dof: solver.compute_free_surface_elevation(grid, rad) for rad in radiation_result}
    radiation_elevation=sum(rao_result.sel(radiating_dof=dof).data * radiation_dof[dof] for dof in all_buoys.dofs)
    # potential=solver.compute_potential(points, diffraction_result) 
    wave_field= incoming_waves_elevation + diffraction_elevation + radiation_elevation
    
    
    circle_coords = [(0, 0), (0, 40), (40, 0), (40, 40)]
    circle_numbers = [1, 2, 3, 4]
    
    def plot_with_circles(grid, data, title):
        plt.figure()
        plt.pcolormesh(grid[0], grid[1], np.real(data))
        plt.xlabel("x")
        plt.ylabel("y")
        cbar = plt.colorbar()
        cbar.set_label('Elevation (m)')
        plt.title(title)
    
        # Add white-filled circles
        for (x, y), num in zip(circle_coords, circle_numbers):
            plt.scatter(x, y, color='white', edgecolor='black', s=200, zorder=5)
            plt.text(x, y, str(num), color='black', fontsize=12, ha='center', va='center', zorder=6)


   

   # Plot each of the figures with circles
    plot_with_circles(grid, incoming_waves_elevation, 'Incoming waves elevation')
    plot_with_circles(grid, diffraction_elevation, 'Diffraction elevation')
    plot_with_circles(grid, radiation_elevation, 'Radiation elevation')
    plot_with_circles(grid, wave_field, 'Total wave field')

    
    # fig = plt.figure()
    # ax = fig.add_subplot(111, projection='3d')
    # counter = range(-30,70)
    # counter2=range(0,100)
    # x,y,z = np.meshgrid(counter, counter, counter2)
    # pnt3d=ax.scatter(x,y,z, c=potential.flat)
    # cbar=plt.colorbar(pnt3d)
    # cbar.set_label("Potential")
    # plt.xlabel("x")
    # plt.ylabel("y")
    # ax.set_zlabel("z")
    # print(potential.flat)
    plt.show()

