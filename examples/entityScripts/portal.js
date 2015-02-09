(function(){ 
    var teleport;
    var portalDestination;

    function playSound() {
      Audio.playSound(teleport, { volume: 0.40, localOnly: true });
    };
    
    this.preload = function(entityID) {
        teleport = SoundCache.getSound("http://s3.amazonaws.com/hifi-public/birarda/teleport.raw");
        
        var properties = Entities.getEntityProperties(entityID);
        portalDestination = properties.userData;
        
        print("The portal destination is " + portalDestination);
    }

    this.enterEntity = function(entityID) {
      if (portalDestination.length > 0) {
        print("Teleporting to hifi://" + portalDestination);
        Window.location = "hifi://" + portalDestination;
      }
      
    }; 
    
    this.leaveEntity = function(entityID) {
      Entities.editEntity(entityID, {
        animationURL: "http://hifi-public.s3.amazonaws.com/models/content/phonebooth.fbx",
        animationSettings: '{ "frameIndex": 1, "running": false }'
      });
      
      playSound();
    };
    
    this.hoverEnterEntity = function(entityID) {
      Entities.editEntity(entityID, {
        animationURL: "http://hifi-public.s3.amazonaws.com/models/content/phonebooth.fbx",
        animationSettings: '{ "fps": 24, "firstFrame": 1, "lastFrame": 25, "frameIndex": 1, "running": true, "hold": true }'
      });
    };
})