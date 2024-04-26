# Import statements
import numpy as np                                                          # numpy for vector processing
import xarray as xr                                                         # xarray for array processing (complex)
# Capytaine import statements
import capytaine as cpt   
import xarray                                                  # capytaine module
from capytaine.post_pro import rao       
                                   # Excitation transfer function response amplitude operator (RAO)
# Legacy Capytaine import statements
from capytaine.io.legacy import export_hydrostatics                         # Export hydrostatics
from capytaine.io.legacy import write_dataset_as_tecplot_files              # Write output data into Tecplot files
from capytaine.io.xarray import separate_complex_values                     # Improved netcdf data export
from capytaine.io.xarray import merge_complex_values                        # Improved netcdf data export
import matplotlib.pyplot as plt
from capytaine.io.mesh_loaders import load_STL


# Import logging module and set to 'debug' level
import logging
logging.basicConfig(level=logging.INFO)
mesh=cpt.load_mesh("D:\Rhino\cylinder.STL", file_format="stl")
# radius=2, height=10

cylinder=cpt.FloatingBody(mesh=mesh)
cylinder.center_of_mass=np.array([0,0,0])
cylinder.rotation_center=np.array([0,0,0])
cylinder.add_all_rigid_body_dofs()
cylinder.keep_immersed_part()
cylinder.inertia_matrix = cylinder.compute_rigid_body_inertia()
cylinder.hydrostatic_stiffness = cylinder.compute_hydrostatic_stiffness(rho=1025)
cylinder.show()

# Print body particulars
#print("Body name:", segment_1.name)
#print("Body dofs:", list(segment_1.dofs.keys()))

# Define range of frequencies as a Numpy array
omega_range = np.linspace(0.01,10,100)

# Define the radiation and diffraction problems for the floating body
problems = [cpt.RadiationProblem(body=cylinder, radiating_dof=dof, omega=omega)
            for dof in cylinder.dofs
            for omega in omega_range]
problems += [cpt.DiffractionProblem(body=cylinder, wave_direction=0.0, omega=omega)
             for omega in omega_range]

# Solve the Capytaine problem
solver = cpt.BEMSolver()
results = solver.solve_all(problems)
dataset = cpt.assemble_dataset(results)
rao_result= rao(dataset)

# Export output data
separate_complex_values(dataset).to_netcdf("D:/capytaine_data/cy_dataset.nc",
                  encoding={'radiating_dof': {'dtype': 'U'},
                            'influenced_dof': {'dtype': 'U'}})
dataset = merge_complex_values(xr.open_dataset("D:/capytaine_data/cy_dataset.nc"))
export_hydrostatics("D:/capytaine_data",cylinder)

# export rao
def save_complex(data_array, *args, **kwargs):
    ds = xarray.Dataset({'real': data_array.real, 'imag': data_array.imag})
    return ds.to_netcdf(*args, **kwargs)
save_complex(rao_result,"D:/capytaine_data/cy_rao.nc")
# separate_complex_values(rao_result).to_netcdf("D:/capytaine_data/cy_rao.nc")
# df = rao_result.to_dataset(name='rao_result').to_dataframe()
# df.to_csv("D:/capytaine_data/cy_rao.csv",index=True)
# Print output data to console
# print(dataset)
plt.figure()
for dof in cylinder.dofs:
    plt.plot(
        omega_range,
        dataset['added_mass'].sel(radiating_dof=dof, influenced_dof=dof),
        label=dof,
        marker='o',
    )
plt.xlabel('omega')
plt.ylabel('added mass')
plt.legend()
plt.tight_layout()
plt.show()


