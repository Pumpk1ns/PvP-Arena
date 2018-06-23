use basic;
use math;
use uo;
use util;
use polsys;
use npc;
use os;
include "include/NPCUtil";
function main_AI_loop()
  var ev := os::wait_for_event(0);
  var wanders := 60;
  var next_loot := ReadGameClock() + 20;
  EnableMainEvents();
  var rint := 0;
  while(1)
    look_around();
    if(Getproperty("patrol"))
    czypatrol := 1;
    patrol(me);
    else
    endif
    if(ReadGameClock() > next_loot)
      grabloot();
      next_loot := ReadGameClock() + 10;
    endif
    rint := RandomInt(4) + 1;
    if(rint == 4)
      PlaySoundEffect(me, idlesnd1);
    elseif(rint == 3)
      PlaySoundEffect(me, idlesnd2);
    endif
    wanders := wanders +1;
    if(wanders >= 60)
      wanders := 0;
      if(czypatrol)
	ev := patrolmode();
      else
      ev := sleepmode();
      endif
    else
      if(czypatrol)
      ev := os::wait_for_event(1);
      else
      ev := os::wait_for_event(5);
      endif
    endif
    if(ev)
      case(ev.type)
        SYSEVENT_ENTEREDAREA:   if((!ev.source.npctemplate) or (ev.source.script == "employed") || (ev.source.script == "tamed") || (ev.source.script == "townGuard") || (ev.source.script == "redGuard"))
                                  Fight(ev.source);
                                endif
        SYSEVENT_ENGAGED:       powiadompodwladnych(ev) ; Fight(ev.source);
        EVID_ALL_ATTACK_CMD:    Fight(ev.source);
        SYSEVENT_DAMAGED:       powiadompodwladnych(ev) ; Fight(ev.source);
        EVID_HERDING:           powiadompodwladnych(ev) ; Herding(ev);
      endcase
    endif
  endwhile
endfunction

function look_around()
  foreach npc in ListMobilesInLineOfSight(me, HALT_THRESHOLD)
    if((!npc.npctemplate) || (npc.script == "employed") || (npc.script == "tamed") || (npc.script == "townGuard") || (npc.script == "redGuard"))
      Fight(npc);
    endif
  endforeach
endfunction

function EnableMainEvents()
  EnableEvents(EVID_HERDING);
  DisableEvents(SYSEVENT_SPEECH + SYSEVENT_LEFTAREA + SYSEVENT_DISENGAGED + SYSEVENT_OPPONENT_MOVED);
  EnableEvents(SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_ENTEREDAREA, HALT_THRESHOLD);
endfunction

function DisableMainEvents()
  DisableEvents(SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_ENTEREDAREA);
endfunction

function herding(ev)
  var holder := ev.data;
  var lx := holder[1];
  var ly := holder[2];
  var loops := 0;
  var opponent;
  var waittime := 0;
  while(1)
	if(!CloseIn(me, lx, ly))
	  loops := loops + 1;
	  waittime := 0;
	else
	  loops := 0;
	  waittime := 1;
	endif
	if((loops >= 30) or (coordist(me.x, me.y, lx, ly) <= 1))
	  break;
	endif
	ev := wait_for_event(waittime);
	case (ev.type)
      SYSEVENT_DAMAGED: opponent := ev.source;
			  		    SetOpponent(opponent);
					    TurnToward(opponent);
	  SYSEVENT_ENGAGED: opponent := ev.source;
			  		    SetOpponent(opponent);
					    TurnToward(opponent);
	endcase
  endwhile
  Return;
endfunction

function powiadompodwladnych(ev)
var osoba;
foreach osoba in Getproperty("podwladni")
 osoba := Systemfindobjectbyserial(osoba);
 Sendevent(osoba, ev);
endforeach
endfunction

function CloseIn(me, lx, ly)
  case (coordist(me.x, me.y, lx, ly))
    0:       return 1;
    default: WalkTowardLocation(lx, ly);
             return 0;
  endcase
endfunction  