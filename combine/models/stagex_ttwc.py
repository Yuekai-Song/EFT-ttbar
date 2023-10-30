from HiggsAnalysis.CombinedLimit.PhysicsModel import *
import fnmatch
from math import sqrt,copysign 
sign = lambda x: copysign(1, x) 
#y0 = (-1.177641)
class stagex_ttwc(PhysicsModel):
    "Allow different signal strength fits for the stage-x model"
    def __init__(self):
        self.POIs = ""
        self.acnorm = False
        self.acmu = False 
        self.acfcp = False 
        self.pois = []

    def setModelBuilder(self, modelBuilder):
        PhysicsModel.setModelBuilder(self, modelBuilder)
        self.modelBuilder.doModelBOnly = False

    def getYieldScale(self,bin,process):
        if "ttbar" not in process: # return 1 for background
            return 1
        else:
            print (process, "is a ttbar process with EW correction")
            if "ci0001" in process:
                muname="r_tt_times_ci0001"
            elif "ci0100" in process:
                muname="r_tt_times_ci0100"
            elif "ci0010" in process:
                muname="r_tt_times_ci0010"
            elif "ci0000" in process:
                muname="r_tt_times_ci0000"
            elif "ci0200" in process:
                muname="r_tt_times_ci0200"
            print ("muname: %s" %muname)
        return muname 

    def setPhysicsOptions(self,physOptions):
        for po in physOptions:
            if 'dofcp' in po:
                self.acfcp= True
                print ("doing ttbar fcp")
            if 'donorm' in po: 
                self.acnorm= True
                print ("using a normalization paramater")
            if 'domu' in po: 
                self.acmu= True
                print ("using mu parameter")

    def doParametersOfInterest(self):
        if not self.acfcp:
            if not self.acnorm:
                if not self.acmu:
                # x:Cpq3, y: Cpu, z:ReCup, k: ImCup
                    self.modelBuilder.doVar("y[0.0,-5,5]" )
                    self.modelBuilder.doVar("z[0.0,-5,5]" )
                    self.modelBuilder.doVar("k[0.0,-5,5]" )
                    self.modelBuilder.factory_("expr::r_tt_times_ci0001(\"@0*@0\", k)")
                    self.modelBuilder.factory_("expr::r_tt_times_ci0100(\"2*@0-@0*@0\", y)")
                    self.modelBuilder.factory_("expr::r_tt_times_ci0200(\"0.5*@0*@0-0.5*@0\", y)")		
                    self.modelBuilder.factory_("expr::r_tt_times_ci0010(\"2*@0-@0*@0\", z)")
                    self.modelBuilder.factory_("expr::r_tt_times_ci0000(\"-@0*@0-1.5*@1+0.5*@1*@1+(@2-1)*(@2-1)\", k, y, z)")
                    self.pois.append("y,z,k")
                else:
                    self.modelBuilder.doVar("y[0.0,-5,5]" )
                    self.modelBuilder.doVar("z[0.0,-5,5]" )
                    self.modelBuilder.doVar("k[0.0,-5,5]" )
                    self.modelBuilder.doVar("mu[1.0,0,10]" )
                    self.modelBuilder.factory_("expr::r_tt_times_ci0001(\"@0*@0\", k)")
                    self.modelBuilder.factory_("expr::r_tt_times_ci0100(\"2*@0-@0*@0\", y)")
                    self.modelBuilder.factory_("expr::r_tt_times_ci0200(\"0.5*@0*@0-0.5*@0\", y)")
                    self.modelBuilder.factory_("expr::r_tt_times_ci0010(\"@0-(@1-1)*(@1-1)\", mu, z)")
                    self.modelBuilder.factory_("expr::r_tt_times_ci0000(\"-@0*@0-1.5*@1+0.5*@1*@1+(@2-1)*(@2-1)\", k, y, z)")
                    self.pois.append("y,z,k")
            else:
                self.modelBuilder.doVar("y[0.0,-5,5]" )
                self.modelBuilder.doVar("z[0.0,-5,5]" )
                self.modelBuilder.doVar("k[0.0,-5,5]" )
                self.modelBuilder.doVar("norm[1.0,0,10]" )
                self.modelBuilder.factory_("expr::r_tt_times_ci0001(\"@0*@0*@1\", k, norm)")
                self.modelBuilder.factory_("expr::r_tt_times_ci0100(\"(2*@0-@0*@0)*@1\", y, norm)")
                self.modelBuilder.factory_("expr::r_tt_times_ci0200(\"(0.5*@0*@0-0.5*@0)*@1\", y, norm)")
                self.modelBuilder.factory_("expr::r_tt_times_ci0010(\"(2*@0-@0*@0)*@1\", z, norm)")
                self.modelBuilder.factory_("expr::r_tt_times_ci0000(\"(-@0*@0-1.5*@1+0.5*@1*@1+(@2-1)*(@2-1))*@3\", k, y, z, norm)")
                self.pois.append("y,z,k")
            
        else:
            self.modelBuilder.doVar("y[0.0,-5,5]" )
            self.modelBuilder.doVar("fcp[0.0,-1.0,1.0]")
            self.modelBuilder.doVar("mu[1.0,0,10]")
            self.modelBuilder.factory_("expr::r_tt_times_ci0001(\"abs(@0)*@1*@1\", fcp, mu)")
            self.modelBuilder.factory_("expr::r_tt_times_ci0100(\"2*@0-@0*@0\", y)")
            self.modelBuilder.factory_("expr::r_tt_times_ci0200(\"0.5*@0*@0-0.5*@0\", y)")
            self.modelBuilder.factory_("expr::r_tt_times_ci0010(\"1-@1*@1*(1-abs(@0))\", fcp, mu)")				
            self.modelBuilder.factory_("expr::r_tt_times_ci0000(\"-abs(@0)*@1*@1-1.5*@2+0.5*@2*@2+@1*@1*(1-abs(@0))\", fcp, mu, y)")
            self.pois.append("y,fcp,mu")
            
            #kappa:sign(@1)*sqrt((1-abs(@0))*abs(@1)) @0:fcp @1:mu
            #kappa_t:sign(@0)*sign(@1)*sqrt(abs(@0)*abs(@1)) @0:fcp @1:mu

        self.POIs=",".join(self.pois)
        print ("the parameters of interest: ", self.POIs)
        self.modelBuilder.doSet("POI",self.POIs)

stagex_ttwc = stagex_ttwc()