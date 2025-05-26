import itertools

moves = ['Hide', 'Jump','NEWMOVES']
shoots = ['LongShot', 'SemiAuto', 'ThirtyShot','NEWSHOOT']
sees = ['Scout', 'Track', 'NEWSEES']

# 生成一级类
for cls in moves + shoots + sees:
    print(f"class {cls}Bot : virtual public ... {{}};")

# 生成二级组合类
for combo in itertools.product(moves, shoots):
    print(f"class {combo[0]}{combo[1]}Bot : public {combo[0]}Bot, public {combo[1]}Bot {{}};")

for combo in itertools.product(moves, sees):
    print(f"class {combo[0]}{combo[1]}Bot : public {combo[0]}Bot, public {combo[1]}Bot {{}};")

for combo in itertools.product(shoots, sees):
    print(f"class {combo[0]}{combo[1]}Bot : public {combo[0]}Bot, public {combo[1]}Bot {{}};")

# 生成三级组合类
for combo in itertools.product(moves, shoots, sees):
    print(f"class {''.join(combo)}Bot : public {combo[0]}Bot, public {combo[1]}Bot, public {combo[2]}Bot {{}};")

# class HideBot : virtual public ... {};
# class JumpBot : virtual public ... {};
# class NEWMOVESBot : virtual public ... {};
# class LongShotBot : virtual public ... {};
# class SemiAutoBot : virtual public ... {};
# class ThirtyShotBot : virtual public ... {};
# class LandmineBot : virtual public ... {};
# class ScoutBot : virtual public ... {};
# class TrackBot : virtual public ... {};
# class NEWSEESBot : virtual public ... {};
# class HideLongShotBot : public HideBot, public LongShotBot {};
# class HideSemiAutoBot : public HideBot, public SemiAutoBot {};
# class HideThirtyShotBot : public HideBot, public ThirtyShotBot {};
# class HideLandmineBot : public HideBot, public LandmineBot {};
# class JumpLongShotBot : public JumpBot, public LongShotBot {};
# class JumpSemiAutoBot : public JumpBot, public SemiAutoBot {};
# class JumpThirtyShotBot : public JumpBot, public ThirtyShotBot {};
# class JumpLandmineBot : public JumpBot, public LandmineBot {};
# class NEWMOVESLongShotBot : public NEWMOVESBot, public LongShotBot {};
# class NEWMOVESSemiAutoBot : public NEWMOVESBot, public SemiAutoBot {};
# class NEWMOVESThirtyShotBot : public NEWMOVESBot, public ThirtyShotBot {};
# class NEWMOVESLandmineBot : public NEWMOVESBot, public LandmineBot {};
# class HideScoutBot : public HideBot, public ScoutBot {};
# class HideTrackBot : public HideBot, public TrackBot {};
# class HideNEWSEESBot : public HideBot, public NEWSEESBot {};
# class JumpScoutBot : public JumpBot, public ScoutBot {};
# class JumpTrackBot : public JumpBot, public TrackBot {};
# class JumpNEWSEESBot : public JumpBot, public NEWSEESBot {};
# class NEWMOVESScoutBot : public NEWMOVESBot, public ScoutBot {};
# class NEWMOVESTrackBot : public NEWMOVESBot, public TrackBot {};
# class NEWMOVESNEWSEESBot : public NEWMOVESBot, public NEWSEESBot {};
# class LongShotScoutBot : public LongShotBot, public ScoutBot {};
# class LongShotTrackBot : public LongShotBot, public TrackBot {};
# class LongShotNEWSEESBot : public LongShotBot, public NEWSEESBot {};
# class SemiAutoScoutBot : public SemiAutoBot, public ScoutBot {};
# class SemiAutoTrackBot : public SemiAutoBot, public TrackBot {};
# class SemiAutoNEWSEESBot : public SemiAutoBot, public NEWSEESBot {};
# class ThirtyShotScoutBot : public ThirtyShotBot, public ScoutBot {};
# class ThirtyShotTrackBot : public ThirtyShotBot, public TrackBot {};
# class ThirtyShotNEWSEESBot : public ThirtyShotBot, public NEWSEESBot {};
# class LandmineScoutBot : public LandmineBot, public ScoutBot {};
# class LandmineTrackBot : public LandmineBot, public TrackBot {};
# class LandmineNEWSEESBot : public LandmineBot, public NEWSEESBot {};
# class HideLongShotScoutBot : public HideBot, public LongShotBot, public ScoutBot {};
# class HideLongShotTrackBot : public HideBot, public LongShotBot, public TrackBot {};
# class HideLongShotNEWSEESBot : public HideBot, public LongShotBot, public NEWSEESBot {};
# class HideSemiAutoScoutBot : public HideBot, public SemiAutoBot, public ScoutBot {};
# class HideSemiAutoTrackBot : public HideBot, public SemiAutoBot, public TrackBot {};
# class HideSemiAutoNEWSEESBot : public HideBot, public SemiAutoBot, public NEWSEESBot {};
# class HideThirtyShotScoutBot : public HideBot, public ThirtyShotBot, public ScoutBot {};
# class HideThirtyShotTrackBot : public HideBot, public ThirtyShotBot, public TrackBot {};
# class HideThirtyShotNEWSEESBot : public HideBot, public ThirtyShotBot, public NEWSEESBot {};
# class HideLandmineScoutBot : public HideBot, public LandmineBot, public ScoutBot {};
# class HideLandmineTrackBot : public HideBot, public LandmineBot, public TrackBot {};
# class HideLandmineNEWSEESBot : public HideBot, public LandmineBot, public NEWSEESBot {};
# class JumpLongShotScoutBot : public JumpBot, public LongShotBot, public ScoutBot {};
# class JumpLongShotTrackBot : public JumpBot, public LongShotBot, public TrackBot {};
# class JumpLongShotNEWSEESBot : public JumpBot, public LongShotBot, public NEWSEESBot {};
# class JumpSemiAutoScoutBot : public JumpBot, public SemiAutoBot, public ScoutBot {};
# class JumpSemiAutoTrackBot : public JumpBot, public SemiAutoBot, public TrackBot {};
# class JumpSemiAutoNEWSEESBot : public JumpBot, public SemiAutoBot, public NEWSEESBot {};
# class JumpThirtyShotScoutBot : public JumpBot, public ThirtyShotBot, public ScoutBot {};
# class JumpThirtyShotTrackBot : public JumpBot, public ThirtyShotBot, public TrackBot {};
# class JumpThirtyShotNEWSEESBot : public JumpBot, public ThirtyShotBot, public NEWSEESBot {};
# class JumpLandmineScoutBot : public JumpBot, public LandmineBot, public ScoutBot {};
# class JumpLandmineTrackBot : public JumpBot, public LandmineBot, public TrackBot {};
# class JumpLandmineNEWSEESBot : public JumpBot, public LandmineBot, public NEWSEESBot {};
# class NEWMOVESLongShotScoutBot : public NEWMOVESBot, public LongShotBot, public ScoutBot {};
# class NEWMOVESLongShotTrackBot : public NEWMOVESBot, public LongShotBot, public TrackBot {};
# class NEWMOVESLongShotNEWSEESBot : public NEWMOVESBot, public LongShotBot, public NEWSEESBot {};
# class NEWMOVESSemiAutoScoutBot : public NEWMOVESBot, public SemiAutoBot, public ScoutBot {};
# class NEWMOVESSemiAutoTrackBot : public NEWMOVESBot, public SemiAutoBot, public TrackBot {};
# class NEWMOVESSemiAutoNEWSEESBot : public NEWMOVESBot, public SemiAutoBot, public NEWSEESBot {};
# class NEWMOVESThirtyShotScoutBot : public NEWMOVESBot, public ThirtyShotBot, public ScoutBot {};
# class NEWMOVESThirtyShotTrackBot : public NEWMOVESBot, public ThirtyShotBot, public TrackBot {};
# class NEWMOVESThirtyShotNEWSEESBot : public NEWMOVESBot, public ThirtyShotBot, public NEWSEESBot {};
# class NEWMOVESLandmineScoutBot : public NEWMOVESBot, public LandmineBot, public ScoutBot {};
# class NEWMOVESLandmineTrackBot : public NEWMOVESBot, public LandmineBot, public TrackBot {};
# class NEWMOVESLandmineNEWSEESBot : public NEWMOVESBot, public LandmineBot, public NEWSEESBot {};