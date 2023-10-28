#include "Event.hpp"

EventType parseEventType(const std::string& eventIdentifier){
    return eventIdentifierMap.at(eventIdentifier);
}