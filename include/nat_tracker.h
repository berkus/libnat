namespace uia
namespace nat

/**
 * This class keeps track of all gateways with NATs and opens, renews and closes port mappings
 * using a multitude of available and supported methods.
 */
class tracker
{
    std::vector<shared_ptr<nat::device>> nats_;
    //mappings_; // Track mappings with timeouts.
};
