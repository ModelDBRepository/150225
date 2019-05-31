%*****************************************************************************************
%           Launch m file for Simulink Cerebellar Abstract with an applied external Force 
%****************************************************************************************   

%% Initializing parameters
% cleaning all the possible variables 
     clear all
     close all
     % Simulation  parameters of the simulink file 
     TrajectoryTime=1;%1sec
     TimeStep=0.002; % Step Size
     SimulationTime=10; %Total simulation time
     
     % This is for storing into a File all the obtained results from 
     % the Simulink file
     FileSuffix = 'AllLearningExternalForces';
     TorqueFile = strcat('CorrectiveTorque',FileSuffix,'.mat');
     TorqueIndFile = strcat('CorrectiveTorque1',FileSuffix,'.mat');
     PCActivityFile = strcat('PCActivity',FileSuffix,'.mat');
     MFDCNWeightFile = strcat('MFDCNWeight',FileSuffix,'.mat');
     PCDCNWeightFile = strcat('PCDCNWeight',FileSuffix,'.mat');
     IODCNWeightFile = strcat('IODCNWeight',FileSuffix,'.mat');
     KPWeightFile = strcat('KPWeight',FileSuffix,'.mat');
     IOActivityFile = strcat('IOActivity',FileSuffix,'.mat');
     ErrorPositionFile = strcat('ErrorPosition',FileSuffix,'.mat');
     Delay=0;
     
     %Parameters for plotting the results 
     NumStep=round(TrajectoryTime/TimeStep);
     NumTrajectories=round(SimulationTime/TrajectoryTime);
    %% Robot load
     load MANIPULATORS
    %load the manipulator file with 05kg mass at the ending efector
     RRedKuKa=RRedKuKa;
     RRedKuKadet=RRedKuKadet05;
     
         %% Simulink simulation framework
     % launch the simulink file, at least the path an time per trajectory 
     % should be specified.
     open_system('CerebellumIODCNAbstractFext');
     set_param('CerebellumIODCNAbstractFext/ToFileTorque','Filename',TorqueFile);
     set_param('CerebellumIODCNAbstractFext/ToFileTorqueInd','Filename',TorqueIndFile);
     set_param('CerebellumIODCNAbstractFext/ToFilePC','Filename',PCActivityFile);
     set_param('CerebellumIODCNAbstractFext/ToFileMFDCN','Filename',MFDCNWeightFile);
     set_param('CerebellumIODCNAbstractFext/ToFilePCDCN','Filename',PCDCNWeightFile);
     set_param('CerebellumIODCNAbstractFext/Subsystem/ErrorModel/ToFileErrorPos','Filename',ErrorPositionFile);
     set_param('CerebellumIODCNAbstractFext/ToFileIO','Filename',IOActivityFile);
     set_param('CerebellumIODCNAbstractFext/ToFileKP','Filename',KPWeightFile);
     set_param('CerebellumIODCNAbstractFext/ToFileIODCN','Filename',IODCNWeightFile);
     open_system('CerebellumIODCNAbstractFext/Subsystem');
     open_system('CerebellumIODCNAbstractFext/Subsystem/ErrorModel');
     
     save_system('CerebellumIODCNAbstractFext');
     mkdir('Register');
     sim('CerebellumIODCNAbstractFext',SimulationTime+Delay);
     
     %% Plots
     Torque = load(TorqueFile);
     Torque = Torque.ans;
    % Torque Values 
     figure(1)
     subplot(3,1,1)
     title('Corrective Torque Values')
     hold on
     plot(Torque(1,:),Torque(2,:),'b')
     hold on
     plot(Torque(1,:),Torque(3,:),'r')
     hold on
     plot(Torque(1,:),Torque(4,:),'k')
     hold on
     subplot(3,1,2)
     hold on
     title('Ideal Torque Values')
     plot(Torque(1,:),Torque(8,:),'b')
     hold on
     plot(Torque(1,:),Torque(9,:),'r')
     hold on
     plot(Torque(1,:),Torque(6,:),'k')
     subplot(3,1,3)
     hold on
     title('Error Torque Values')
     plot(Torque(1,:),Torque(8,:)-Torque(2,:),'b')
     hold on
     plot(Torque(1,:),Torque(9,:)-Torque(3,:),'r')
     hold on
     plot(Torque(1,:),Torque(10,:)-Torque(4,:),'k')
     clear Torque;

      % Partial Torque Values 
     Torque1 = load(TorqueIndFile);
     Torque1 = Torque1.ans;
     figure(2)
     subplot(3,1,1)
     hold on
     title('Corrective partial Torque Values')
     hold on
     plot(Torque1(1,:),Torque1(2,:),'b')
     hold on
     plot(Torque1(1,:),Torque1(3,:),'r')
     legend('torque +q1', 'torque -q1')
     hold on
     subplot(3,1,2)
     hold on
     title('Corrective partial Torque Values')
     hold on
     plot(Torque1(1,:),Torque1(4,:),'b')
     hold on
     plot(Torque1(1,:),Torque1(5,:),'r')
     legend('torque +q2', 'torque -q2')
     hold on
     subplot(3,1,3)
     hold on
     title('Corrective partial Torque ')
     hold on
     plot(Torque1(1,:),Torque1(6,:),'b')
     hold on
     plot(Torque1(1,:),Torque1(7,:),'r')
     legend('torque +q3', 'torque -q3')
     hold on
     
     % Stiffnes     
     figure(3)
     subplot(3,1,1)
     title('Stiffness Joint 1')
     hold on
     plot(Torque1(1,:),Torque1(2,:)+Torque1(3,:),'b')
     hold on
     subplot(3,1,2)
     title('Stiffness Joint 2')
     hold on
     plot(Torque1(1,:),Torque1(4,:)+Torque1(5,:),'r')
     hold on
     subplot(3,1,3)
     title('Stiffness Joint 3')
     hold on
     plot(Torque1(1,:),Torque1(6,:)+Torque1(7,:),'k')
     hold on
     clear Torque1;
     
     % Activity at Purkinje Cells
     PCActivity = load(PCActivityFile);
     PCActivity = PCActivity.ans;
     figure(4)
     subplot(3,1,1)
     hold on
     title('PC Activity Joint 1')
     plot(PCActivity(1,:),PCActivity(2,:),'b')
     hold on
     title('PC Activity Joint 1')
     plot(PCActivity(1,:),PCActivity(3,:),'r')
     legend('+Q1', '-Q1')
     hold on
     subplot(3,1,2)
     hold on
     title('PC Activity Joint 2')
     plot(PCActivity(1,:),PCActivity(4,:),'b')
     hold on
     title('PC Activity Joint 2')
     plot(PCActivity(1,:),PCActivity(5,:),'r')
     legend('+Q2', '-Q2')
     hold on
     subplot(3,1,3)
     hold on
     title('PC Activity Joint 3')
     plot(PCActivity(1,:),PCActivity(6,:),'b')
     hold on
     title('PC Activity Joint 3')
     plot(PCActivity(1,:),PCActivity(7,:),'r')
     legend('+Q3', '-Q3')
     hold on
     clear PCActivity;
     
      % Mossy Fiber -Deep Cerebellar Nuclei synaptic weights
     MFDCNWeight = load(MFDCNWeightFile);
     MFDCNWeight = MFDCNWeight.ans;

     figure(5)
     subplot(3,1,1)
     title('MFDCN Weight Joint 1')
     hold on
     plot(MFDCNWeight(1,:),MFDCNWeight(2,:),'b')
     hold on
     title('MFDCN Weight Joint 1')
     plot(MFDCNWeight(1,:),MFDCNWeight(3,:),'r')
     legend('+Q1', '-Q1')
     hold on
     subplot(3,1,2)
     title('MFDCN Weight Joint 2')
     plot(MFDCNWeight(1,:),MFDCNWeight(4,:),'b')
     hold on
     title('MFDCN Weight Joint 2')
     plot(MFDCNWeight(1,:),MFDCNWeight(5,:),'r')
     legend('+Q2', '-Q2')
     hold on
     subplot(3,1,3)
     hold on
     title('MFDCN Weight Joint 3')
     plot(MFDCNWeight(1,:),MFDCNWeight(6,:),'b')
     hold on
     title('MFDCN Weight Joint 3')
     plot(MFDCNWeight(1,:),MFDCNWeight(7,:),'r')
     legend('+Q3', '-Q3')
     hold on
     clear MFDCNWeight;
     
     % Purkinje Cell- Deep Cerebellar Nuclei synaptic weights 
     PCDCNWeight = load(PCDCNWeightFile);
     PCDCNWeight = PCDCNWeight.ans;
     
     figure(6)
     subplot(3,1,1)
     title('PCDCN Weight Joint 1')
     hold on
     plot(PCDCNWeight(1,:),PCDCNWeight(2,:),'b')
     hold on
     title('PCDCN Weight Joint 1')
     plot(PCDCNWeight(1,:),PCDCNWeight(3,:),'r')
     legend('+Q1', '-Q1')
     hold on
     subplot(3,1,2)
     title('PCDCN Weight Joint 2')
     plot(PCDCNWeight(1,:),PCDCNWeight(4,:),'b')
     hold on
     title('PCDCN Weight Joint 2')
     plot(PCDCNWeight(1,:),PCDCNWeight(5,:),'r')
     legend('+Q2', '-Q2')
     hold on
     subplot(3,1,3)
     title('PCDCN Weight Joint 3')
     plot(PCDCNWeight(1,:),PCDCNWeight(6,:),'b')
     hold on
     title('PCDCN Weight Joint 3')
     plot(PCDCNWeight(1,:),PCDCNWeight(7,:),'r')
     legend('+Q3', '-Q3')
     hold on
     clear PCDCNWeight;
      
     % Inferior Olive -Deep Cerebellar Nuclei synaptic weights
     
     IODCNWeight = load(IODCNWeightFile);
     IODCNWeight = IODCNWeight.ans;
    
     figure(7)
     subplot(3,1,1)
     title('IODCN Weight Joint 1')
     hold on
     plot(IODCNWeight(1,:),IODCNWeight(2,:),'b')
     hold on
     title('IODCN Weight Joint 1')
     plot(IODCNWeight(1,:),IODCNWeight(3,:),'r')
     legend('+Q1', '-Q1')
     hold on
     subplot(3,1,2)
     title('IODCN Weight Joint 2')
     plot(IODCNWeight(1,:),IODCNWeight(4,:),'b')
     hold on
     title('IODCN Weight Joint 2')
     plot(IODCNWeight(1,:),IODCNWeight(5,:),'r')
     legend('+Q2', '-Q2')
     hold on
     subplot(3,1,3)
     title('IODCN Weight Joint 3')
     plot(IODCNWeight(1,:),IODCNWeight(6,:),'b')
     hold on
     title('IODCN Weight Joint 3')
     plot(IODCNWeight(1,:),IODCNWeight(7,:),'r')
     legend('+Q3', '-Q3')
     hold on
     
     
     % kp synaptic weights Evolution
     KPWeight = load(KPWeightFile);
     KPWeight = KPWeight.ans;
     
     figure(8)
     subplot(3,1,1)
     title('KP Weight Joint 1')
     hold on
     plot(KPWeight(1,:),KPWeight(2,:),'b')
     hold on
     title('KP Weight Joint 1')
     plot(KPWeight(1,:),KPWeight(3,:),'r')
     legend('+Q1', '-Q1')
     hold on
     subplot(3,1,2)
     title('KP Weight Joint 2')
     plot(KPWeight(1,:),KPWeight(4,:),'b')
     hold on
     title('KP Weight Joint 2')
     plot(KPWeight(1,:),KPWeight(5,:),'r')
     legend('+Q2', '-Q2')
     hold on
     subplot(3,1,3)
     title('KP Weight Joint 3')
     plot(KPWeight(1,:),KPWeight(6,:),'b')
     hold on
     title('KP Weight Joint 3')
     plot(KPWeight(1,:),KPWeight(7,:),'r')
     legend('+Q3', '-Q3')
     hold on
     
     % KP*Inferior Olive -Deep Cerebellar Nuclei synaptic weights
     figure(9)
     subplot(3,1,1)
     title('KP*IODCN Weight Joint 1')
     hold on
     plot(KPWeight(1,:),KPWeight(2,:).*IODCNWeight(2,:),'b')
     hold on
     title('KP*IODCN Weight Joint 1')
     plot(KPWeight(1,:),KPWeight(3,:).*IODCNWeight(3,:),'r')
     legend('+Q1', '-Q1')
     hold on
     subplot(3,1,2)
     title('KP*IODCN Weight Joint 2')
     plot(KPWeight(1,:),KPWeight(4,:).*IODCNWeight(4,:),'b')
     hold on
     title('KP*IODCN Weight Joint 2')
     plot(KPWeight(1,:),KPWeight(5,:).*IODCNWeight(5,:),'r')
     legend('+Q2', '-Q2')
     hold on
     subplot(3,1,3)
     title('KP*IODCN Weight Joint 3')
     plot(KPWeight(1,:),KPWeight(6,:).*IODCNWeight(6,:),'b')
     hold on
     title('KP*IODCN Weight Joint 3')
     plot(KPWeight(1,:),KPWeight(7,:).*IODCNWeight(7,:),'r')
    legend('+Q3', '-Q3')
     hold on
      
     clear KPWeight;
     clear IODCNWeight;
    
     % Position Error Evolution    
     ErrorPos = load(ErrorPositionFile);
     ErrorPos = ErrorPos.ans;
     
     figure(10)
     title('Position Error')
     plot(ErrorPos(1,:),ErrorPos(2,:),'b')
     hold on
     plot(ErrorPos(1,:),ErrorPos(3,:),'r')
     hold on
     plot(ErrorPos(1,:),ErrorPos(4,:),'k')
     legend('joint 1', 'joint 2','joint3')
     
     
     ErrorPos1=ErrorPos(2,:)';
     ErrorPos2=ErrorPos(3,:)';
     ErrorPos3=ErrorPos(4,:)';
     clear ErrorPos;       

     for i=1:NumTrajectories,
           Mae1(i)=mae(ErrorPos1(NumStep*(i-1)+1:(NumStep*(i))-1));
           Mae2(i)=mae(ErrorPos2(NumStep*(i-1)+1:(NumStep*(i))-1));
           Mae3(i)=mae(ErrorPos3(NumStep*(i-1)+1:(NumStep*(i))-1));
     end
      figure(11)
     subplot(3,1,1)
     title('MAE error Joint1 (Position)')
     hold on
     plot(Mae1,'b')
     hold on
     subplot(3,1,2)
     title('MAE error Joint2 (Position)')
     hold on
     plot(Mae2,'r')
     hold on
     subplot(3,1,3)
     title('MAE error Joint3 (Position)')
     hold on
     plot(Mae3,'k')
     hold on
    
     save_system('CerebellumIODCNAbstractFext');
     close_system('CerebellumIODCNAbstractFext');