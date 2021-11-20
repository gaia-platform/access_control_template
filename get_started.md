# Gaia Access Control sample 

The Gaia Sandbox provides a containerized simulation environment that allows you to experiment with samples written using the Gaia Platform.

This sample models a security access control to allow or deny people entering an office campus. It contains:

- A headquarters (HQ) building
- Rooms inside the HQ building
- People, of varying security access levels
    - John: an employee with access to all rooms
    - Jane: a visitor with limited room access
    - A stranger: they are allowed inside the HQ building!
- Event schedules for people and rooms

## Overview of the Sandbox

The Sandbox UI consists of the following elements.

### User interface window

The right side of the screen shows the controls for the Access Control simulation. Each person in the **People** column has several icons: **Badge**, **Face**, **Parking**, and **WiFi**. These icons are all scanning options: for example, clicking on somebody's **Badge** icon simulates a "badge scan" into a building.

Every person has a drop-down menu of the buildings/rooms they can scan into. Click on that box to choose a building/room, then click on one of the scan icons to choose a way to scan into that building/room. Every drop-down menu starts with only the **HQ Building** because you must scan into a building before you can scan into a room.

You can also control time! Click the fast-forward icon by **Location at time** to set the time, which is important for visitors like Jane who cannot enter a room until a certain time.

### Ruleset, DDL, and Output tabs

There are three tabs on the left. These are windows that show:

- **Ruleset:** Declarative C++ code that handles all the "business logic" of building and room access. You can edit the code in this tab and rebuild the sample to see how it affects the way that the sample runs.
- **DDL:** A database schema describing the "data model" of everything in this Access Control scenario.
- **Output:** Displays details while building/running your program.

## Build and run the sample

For the best user experience, keep the Sandbox's browser maximized to full-screen mode and use 1080p or higher resolution.

The Sandbox contains a complete sample for you to examine and modify.

To build the sample, select the **Build** button located above the simulator window. The state of the button changes depending on what point in the build/run cycle that you are in.

While the sample builds, the **Output** window displays the progress of the build. **It can take several minutes** to build the sample.

When the sample has completed building, press **Run**.

### Example interaction
1. Build and run the sample.
2. Click John's **Badge** icon to scan his badge at the HQ building.
3. Click John's **Face** icon to scan his face at the HQ building and let him in.
4. Click Jane's **Badge** then **Face** icon. She will not be allowed in!
5. Click the **Fast-forward** button until it reads `10:00 am`.
6. Again, click Jane's **Badge** then **Face** icon and she will be let into the HQ building!
7. Change John and Jane's drop-down boxes to say `Big Room` instead of `Little Room`.
8. Click John and Jane's **Face** icons to scan them into the Big Room for their "boring meeting".
9. Try other interactions!

Now you can start experimenting with the sample to see how it affects the running of the factory. You can modify the rules and DDL. After you rebuild the sample, run it and see the new effects.

For more information about Rulesets and Gaia DDL, see the [Gaia technical documentation](http://docs.gaiaplatform.io). 