#include "goby/zeromq/single-thread-application.h"

#include "messages/groups.h"
#include "messages/state.pb.h"

#include "config.pb.h"

using Base = goby::zeromq::SingleThreadApplication<StateMachineCtrlConfig>;

class StateMachineCtrl : public Base
{
  public:
    StateMachineCtrl() : Base(1.0 * boost::units::si::hertz)
    {
        ctrl_.set_desired_state(protobuf::ON);

        interprocess().subscribe<groups::state_report, protobuf::StateReport>(
            [](const protobuf::StateReport& report) {
                std::cout << "Received state report: " << report.ShortDebugString() << std::endl;
            });
    }

  private:
    void loop() override
    {
        // toggle the desired state
        if (ctrl_.desired_state() == protobuf::ON)
            ctrl_.set_desired_state(protobuf::OFF);
        else if (ctrl_.desired_state() == protobuf::OFF)
            ctrl_.set_desired_state(protobuf::ON);

        std::cout << "Tx: " << ctrl_.DebugString() << std::flush;
        interprocess().publish<groups::state_control>(ctrl_);
    }

  private:
    protobuf::StateControl ctrl_;
};

int main(int argc, char* argv[]) { return goby::run<StateMachineCtrl>(argc, argv); }
