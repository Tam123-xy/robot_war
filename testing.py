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

# shixiang code

# class MovingRobot : virtual public Robot {
# protected:

#     // bool hasJumpAbility = false;
#     // bool hasHideAbility = false;
#     // int jumpCount = 0;
#     // int hideCount = 0;
#     // bool isHidden = false;

# public:
#     MovingRobot(string name, int x, int y, int w, int h)
#         : Robot(name, x, y, w, h) {}
#     using Robot::Robot;
#     virtual ~MovingRobot() = default;

#     //Basic Movement
#     virtual void move(int dx, int dy) = 0;
    
#     // //Special movement abilities
#     // virtual bool jump(int newX, int newY) {
#     //     if (canJump() && newX > 0 && newX < width && newY > 0 && newY < height) {
#     //         positionX = newX;
#     //         positionY = newY;
#     //         jumpCount++;
#     //         cout << name << "jumped to (" << newX << "," << newY << ") (" << 3 - jumpCount << " jumps left)" << endl;
#     //         return true;
#     //     }
#     //     return false;
#     // }

#     // virtual bool hide() {
#     //     if (canHide()) {
#     //         isHidden = true;
#     //         hideCount++;
#     //         cout << name << " is now hidden (" << 3 - hideCount << " hides left)" << endl;
#     //         return true;
#     //     }
#     //     return false;
#     // }

#     // virtual void reveal() {
#     //     if (isHidden) {
#     //         isHidden = false;
#     //         cout << name << " is now visible again" << endl;
#     //     }
#     // }

#     // //Ability activation
#     // void activateJumpAbility() {
#     //     hasJumpAbility = true;
#     //     cout << name << " gained JumpBot abilities!" << endl;
#     // }

#     // void activateHideAbility() {
#     //     hasHideAbility = true;
#     //     cout << name << " gained HideBot abilities!" << endl;
#     // }

#     // virtual bool defendAgainstAttack() {
#     //     return false;   // Base robots don't defend by default
#     // }

#     // //Status checks
#     // bool canJump() const { return hasJumpAbility && jumpCount < 3; }
#     // bool canHide() const { return hasHideAbility && hideCount < 3; }
#     // bool isCurrentlyHidden() const { return isHidden; }
#     // int getRemainingJumps() const { return 3 - jumpCount; }
#     // int getRemainingHides() const { return 3 - hideCount; }

#     //Movement validation
#     bool isValidMove(int dx, int dy) const {
#         int newX = positionX + dx;
#         int newY = positionY + dy;
#         return (newX > 0 && newX < width && newY > 0 && newY < height);
#     }
# };


# class HideBot : virtual public MovingRobot {
# private:
#     int hideUses = 3;   //Track remaining hides
#     bool isHidden = false;  //Current hidden state
#     bool hideWhenAttacked = true;

# public:
#     HideBot(string name, int x, int y, int w, int h)
#         : Robot(name, x, y, w, h), MovingRobot(name, x, y, w, h) {
#         activateHideAbility();
#     }
    
#     string getType() const override { return "HideBot" ; }

#     //Override hide to provide invulnerability
#     bool hide() override {
#         if (hideUses > 0 && !isHidden) {
#             isHidden = true;
#             hideUses--;
#             return true;
#         }
#         return false;
#     }

#     bool defendAgainstAttack() {
#         if (canHide() && hideWhenAttacked && !isHidden) {
#             return hide();  //Automatically hide when attacked
#         }
#         return false;
#     }

#     void reveal() override {
#         if (isHidden) {
#             isHidden = false;
#         }
#     }

#     // If we have hide ability and choose to use it
#     void move(int dx, int dy) override {
#         if (canHide() && rand() % 2 == 0) { // 50% chance to use hide if available
#             if (hide()) {
#                 return;
#             }
#         }
#     }

#     //Method to check if robot can be hit (for attack logic)
#     bool canBeHit() const override {
#         return !isHidden;   // Can only be hit when not hidden
#     }

#     int getRemainingHides() const { return hideUses; }
#     bool canHide() const { return hideUses > 0; }
#     bool isCurrentlyHidden() const { return isHidden; }
# };

# class JumpBot : virtual public MovingRobot {
# private:
#     int jumpUses = 3;   //Track remaining jumps

# public:
#     JumpBot(string name, int x, int y, int w, int h)
#         : Robot(name, x, y, w, h), MovingRobot(name, x, y, w, h) {
#         activateJumpAbility();
#     }
    
#     string getType() const override { return "JumpBot" ; }

#     // Override jump to allow jumping anywhere on the map
#     bool jump(int newX, int newY) override {
#         if (jumpUses > 0 && newX > 0 && newX < width && newY > 0 && newY < height) {
#             positionX = newX;
#             positionY = newY;
#             jumpUses--;
#             cout << name << " jumped to (" << newX << "," << newY << ")! (" << jumpUses << " jumps remaining)" << endl;
#             return true;
#         }
#         return false;
#     }

#     void move(int dx, int dy) override {
#         //JumpBot can choose to use normal move or jump
#         if (canJump() && rand() % 2 == 0) { // 50% chance to use jump if available
#             int newX = rand() % width;
#             int newY = rand() % height;
#             if (jump(newX, newY)) {
#                 return;
#             }
#         }

#         // Normal movement if jump not used or not available
#         if (isValidMove(dx, dy)) {
#             positionX += dx;
#             positionY += dy;
#             cout << name << " moved to (" << positionX << "," << positionY << ")" << endl;
#         }
#     }

#     int getRemainingJumps() const { return jumpUses; }
#     bool canJump() const { return jumpUses > 0; }
# };


# robots.cpp
#         // Check if target is HideBot and handle defense
#         // if (enemy->getType() == "HideBot") {
#         //     if (enemy->hide()) {
#         //         shells--;  // Still consume a shell
#         //         cout << name << " attacked " << enemy->getName() << ", but target is hidden!" << endl;
#         //         return;
#         //     }
#         // }

#         //Proceed with normal attack if not defended
#         // if (!enemy->canBeHit()) {
#         //     shells--;
#         //     cout << name << " fires " << enemy->getName() 
#         //         << " at (" << targetX << "," << targetY << ")" << " but " 
#         //         << enemy->getName() << " is hidden! Attack wasted." << endl;
#         //     return;
#         // }
