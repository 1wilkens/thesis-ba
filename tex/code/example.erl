%% Module example (this must match the filename - '.erl')
-module(example).
%% This module exports two functions: start and codeswitch
%% The number after each function represents the param count
-export([start/0, codeswitch/1]).

start() -> loop(0).

loop(Sum) ->
  % Match on first received message in process mailbox
  receive
    {increment, Count} ->
      loop(Sum+Count);
    {counter, Pid} ->
      % Send current value of Sum to PID
      Pid ! {counter, Sum},
      loop(Sum);
    code_switch ->
      % Explicitly use the latest version of the function
      % => hot code reload
      ?MODULE:codeswitch(Sum)
end.

codeswitch(Sum) -> loop(Sum).
