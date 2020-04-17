#include "eventmanager.h"

namespace core
{

BaseEvent::Family BaseEvent::family_counter_ = 0;

BaseEvent::~BaseEvent() {
}

EventManager::EventManager() {
}

EventManager::~EventManager() {
}

void EventManager::emitEvent(const BaseEvent &event) {
  auto sig = signal_for(event.my_family());
  sig->emitEvent(&event);
}

}
