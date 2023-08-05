package userinterface;

import py4j.GatewayServer;

public class GateWay{

    public void start(int port){
        GatewayServer gws = new GatewayServer(new UserInterface(), port);
        gws.start();
    }
}