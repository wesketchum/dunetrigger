/**
 * @file compile_me.cxx
 *
 * TODO: Eric Flumerfelt <eflumerf@fnal.gov> May-21-2021: Please explain what this file is for
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "dune-triggers/Supernova/TriggerCandidateMaker_Supernova.hpp"
#include "dune-triggers/Trivial/TriggerCandidateMaker_Trivial.hpp"
#include "dune-triggers/Trivial/TriggerDecisionMaker_Trivial.hpp"
#include <string>

int
main()
{
  std::string s;
  TriggerCandidateMakerTrivial trivial(s);
  (void)trivial;
  TriggerDecisionMakerTrivial trivial2(s);
  (void)trivial2;
  TriggerCandidateMakerSupernova super(s);
  (void)super;

  return 0;
}
