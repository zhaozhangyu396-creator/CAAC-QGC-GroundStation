# ArduCopter 4.7.2 — geofence Brake without automatic landing

This source patch targets **Copter-4.7.2**, commit
`fb773567896014da007bca58358704d4a0d8516f`, for **Pixhawk6C**. On 2026-10-16 the
GitHub release and the official Pixhawk6C `Copter/stable` build both identify this
version. `manifest.json` pins the release, source hashes, output hashes and patch.

Set **FENCE_NOLD=1** together with **FENCE_ACTION=4** to enable the added policy:

- A new fence breach attempts Brake. This also applies beyond the native 100 m
  give-up distance.
- If Brake mode entry is rejected, the existing flight mode remains active and a
  critical MAVLink warning says `Fence brake failed: take control`.
- Repeated or farther breaches follow the same branch and cannot reach the
  geofence RTL/Land fallbacks.
- An airborne pilot moving a manual throttle to zero does not trigger the native
  fence motor-disarm branch in this policy. Grounded disarm remains unchanged.
- The fence mode-change lockout is bypassed for this policy, allowing the pilot to
  select another mode even when FENCE_OPTIONS bit 0 is set. Native manual recovery
  timing remains in effect.

**FENCE_NOLD defaults to 0**. Other fence actions retain their original behavior.
Battery, RC, GCS, EKF and other independent failsafes remain unchanged; they can
still request their configured landing response. A rejected Brake entry means
that this fence cannot physically contain the aircraft, so the operator must
recover it. This patch does not restore a historical mode after an independent
failsafe or after Brake has already been accepted.

FENCE_NOLD is an `AP_Int8`, values 0/1, at unused AC_Fence group index 16. Its
parameter metadata is limited to Copter/Tricopter/Helicopter frames. It identifies
this source adaptation; an unmodified official stable image does not provide it.

## Validation

Run against an exact upstream source checkout:

```sh
python3 custom/firmware/ardupilot/validate.py --source-root /path/to/ardupilot
```

Omit `--source-root` to download the six pinned official source files over HTTPS.
The validator verifies their SHA-256 hashes, runs `git apply --check`, applies the
patch in a temporary directory and verifies the patched hashes. It compiles the
actual patched `ArduCopter/fence.cpp`, actual capability accessor and actual
fence mode-lockout condition into a host harness with test doubles. The harness
covers 92 scenarios: mode-entry rejection in three original modes, opt-out and
invalid opt-in values, the 100 m boundary, successful Brake, 500 repeated/mixed
breaches, pilot-mode lockout, other fence actions, airborne zero-throttle recovery, disarmed and landed handling.

The checked-in result is **patch applicability and host policy scenarios passed**.
A full ArduPilot build, SITL test and Pixhawk6C hardware test have **not** been
performed. No firmware image is supplied or flashed by this change.

## Building the adapted firmware

In an official ArduPilot checkout with its documented toolchain and submodules:

```sh
git checkout fb773567896014da007bca58358704d4a0d8516f
git submodule update --init --recursive
git apply --check /path/to/Copter-4.7.2-caac-no-land.patch
git apply /path/to/Copter-4.7.2-caac-no-land.patch
./waf configure --board Pixhawk6C
./waf copter
```

Perform bench and SITL verification before using an adapted image on aircraft.
The patch changes ArduPilot source and follows its GPLv3 license; it is provided
separately from the QGroundControl executable.
