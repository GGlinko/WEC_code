hydro = struct();

hydro = readCAPYTAINE(hydro,'cy_dataset.nc');
hydro = radiationIRF(hydro,60,[],[],[],[]);
hydro = radiationIRFSS(hydro,[],[]);
hydro = excitationIRF(hydro,60,[],[],[],[]);
writeBEMIOH5(hydro)
plotBEMIO(hydro)

