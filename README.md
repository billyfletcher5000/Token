# Token

This is an Unreal Engine 5 project that's a bit of a test bed and current repository for a plugin I'm working on called **Cadence**. It's primarily just a test environment, the majority of interesting work can be found in the `Plugins/Cadence` folder.

## Cadence

Cadence is an attempt to build a plugin to make the process of choreographing game 'actions' to rhythm and music (the separation is significant, as part of the goal is to have non-audible rhythms and timing cues). 

It attempts to provide a few main features:
- A mechanism for defining rhythmic and section information, whether from audio analysis, MIDI import or manual placement via Unreal's Timeline editor (MIDI still to come)
- Level based authoring of what actors (or other elements) are affected by choreography and how, via the concept of:
  1. 'Stages' - bounded volumes in the world that help organise elements into a stage play style space (and can/will be able to be integrated into level instances/packed level actors)
  2. 'Places' - snapshots of transforms of actors that can be stored and given IDs for choreography, including association with reaction groups below, this allows a similar behaviour to keyframing but allows for more adaptive timing
  3. 'Reaction Groups' - Groups of actors or other elements that can be used to choreograph elements as a single entity, e.g. you may define an actor in a stage to have a number of places it can be at, while you would associate those places with a meaningful 'synchronisation' point within its reaction group.
- A node graph based editor called Choreo Graph, this is largely based on Blueprint but effectively allows multiple active 'flows' at any one time and (hopefully) more intuitively follows a left to right progression in line with the progression of the rhythm or music.

### How well does it actually work?

Err at this stage not particularly well, it's clearly high concept and unlikely to reach it, but technically nearly all the elements above are in and working, including some cool features like the ability to interpole based on rhythm that can be stepped by a smaller subdivision of that rhythmic quantization boundary and the ability to manage flow on the graph for a given 'pathway' where it will wait for defined triggers before executing, allowing you to do a series of movements, wait for the next marked rhythmic event (which can be procedural or authored) before executing another authored set of graph nodes.

Generally just needs some time spent to work out what actually has value, what can be be improved from a usability point of view and what should be discarded/replaced with a different approach. Fundamentally it tries to solve a problem that involves predicting the future, adapting with little warning and tries to be as non-prescriptive of use case as possible, so it's likely to take much more time than I have to spend on it to make it work, but I will continue to try and find time to develop it further.

The ideal end result of the plugin is to enable the sort of choreographing of sound to action that occurs in film, but in a dynamic environment, with tools to help bridge the gap and make it clear where the (many) limits of trying to author time in a media form where it's a thing we generally relinquish to the player.
