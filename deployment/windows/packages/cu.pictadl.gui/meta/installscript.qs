function Component(){
    // default constructor
}

Component.prototype.createOperations = function()
{
    component.createOperations();
    
    if(systemInfo.productType === "windows"){
        component.addOperation("CreateShortcut", "@TargetDir@/Picta-dl_GUI.exe", "@StartMenuDir@/Picta DLG.lnk", "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/Picta-dl_GUI.exe", "iconId=0", "description=Open Picta Downloader GUI");
    }
}
