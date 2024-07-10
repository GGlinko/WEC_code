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
mesh=cpt.load_mesh("D:\Rhino\cylinder.STL", file_format="stl",name="cylinder")
# radius=2, height=10

cylinder1=cpt.FloatingBody(mesh=mesh,name='body1')
cylinder1.center_of_mass=np.array([0,0,0])
cylinder1.rotation_center=np.array([0,0,0])
# cylinder1.add_translation_dof(name="Heave")
cylinder1.add_all_rigid_body_dofs()
cylinder1.keep_immersed_part()
cylinder1.inertia_matrix = cylinder1.compute_rigid_body_inertia()
cylinder1.hydrostatic_stiffness = cylinder1.compute_hydrostatic_stiffness(rho=1025)

cylinder2=cpt.FloatingBody(mesh=mesh,name='body2')
cylinder2.center_of_mass=np.array([20,0,0])
cylinder2.rotation_center=np.array([20,0,0])
cylinder2.add_all_rigid_body_dofs()
cylinder2.keep_immersed_part()
cylinder2.inertia_matrix = cylinder2.compute_rigid_body_inertia()
cylinder2.hydrostatic_stiffness = cylinder2.compute_hydrostatic_stiffness(rho=1025)

cylinder3=cpt.FloatingBody(mesh=mesh,name='body3')
cylinder3.center_of_mass=np.array([20,20,0])
cylinder3.rotation_center=np.array([20,20,0])
cylinder3.add_all_rigid_body_dofs()
cylinder3.keep_immersed_part()
cylinder3.inertia_matrix = cylinder3.compute_rigid_body_inertia()
cylinder3.hydrostatic_stiffness = cylinder3.compute_hydrostatic_stiffness(rho=1025)

cylinder4=cpt.FloatingBody(mesh=mesh,name='body4')
cylinder4.center_of_mass=np.array([0,20,0])
cylinder4.rotation_center=np.array([0,20,0])
cylinder4.add_translation_dof(name='Heave')
cylinder4.keep_immersed_part()
cylinder4.inertia_matrix = cylinder4.compute_rigid_body_inertia()
cylinder4.hydrostatic_stiffness = cylinder4.compute_hydrostatic_stiffness(rho=1025)


all_bodies = cylinder1 + cylinder2 + cylinder3 + cylinder4
all_bodies.hydrostatic_stiffness=all_bodies.compute_hydrostatic_stiffness(rho=1025)
all_bodies.show()

# Define range of frequencies as a Numpy array
omega_range = 0.5

problems = [cpt.RadiationProblem(body=all_bodies, radiating_dof=dof, omega=omega)
            for dof in all_bodies.dofs
            for omega in omega_range]
problems += [cpt.DiffractionProblem(body=all_bodies, wave_direction=0.0, omega=omega)
             for omega in omega_range]

solver = cpt.BEMSolver()
results = solver.solve_all(problems)
dataset = cpt.assemble_dataset(results)

separate_complex_values(dataset).to_netcdf("D:/capytaine_multiple_devices/multiple_cy.nc",
                  encoding={'radiating_dof': {'dtype': 'U'},
                            'influenced_dof': {'dtype': 'U'}})
dataset = merge_complex_values(xr.open_dataset("D:/capytaine_multiple_devices/multiple_cy.nc"))
export_hydrostatics("D:/capytaine_multiple_devices",cylinder1)

