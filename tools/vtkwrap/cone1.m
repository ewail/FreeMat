cone = vtkConeSource;
SetHeight(cone,3);
SetRadius(cone,1);
SetResolution(cone,10);
coneMapper = vtkPolyDataMapper;
SetInputConnection(coneMapper,GetOutputPort(cone));
coneActor = vtkActor;
SetMapper(coneActor,coneMapper);
ren1 = vtkRenderer;
AddActor(ren1,coneActor);
SetBackground(ren1,0.1,0.2,0.4);
renWin = vtkRenderWindow;
AddRenderer(renWin,ren1);
SetSize(renWin,300,300);
keyboard
for i=1:360
	sleep(0.03);
Render(renWin);
Azimuth(GetActiveCamera(ren1),1);
end
