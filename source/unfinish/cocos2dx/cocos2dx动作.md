---
title: cocos2dx动作
---
*《cocos2d-x游戏开发》初学笔记*

## 动作基类Action

> Action是一个抽象类，几乎所有动作都由该类派生而来，Action的派生关系如下图所示：
<div align=center>![cocos基本角色](https://github.com/knightjk/knightjk.github.io/blob/hexo/source/images/cocos2dx%E5%9F%BA%E6%9C%AC%E6%A6%82%E5%BF%B5/manjor_concept.PNG?raw=true)
<div align=left>
> 更详细的信息在 [这里](http://www.cocos2d-x.org/reference/native-cpp/V3.0/db/d61/classcocos2d_1_1_action.html)

> 源码的对应关系如下：
* 动作基类 *Action*
```c
//cocos2d\cocos\2d\CCAction.h
/** 
 * @brief Base class for Action objects.
 */
class CC_DLL Action : public Ref, public Clonable{......};
```
* 有限时间动作 *FiniteTimeAction*
```c
//cocos2d\cocos\2d\CCAction.h
/** @class FiniteTimeAction
 * @brief
 * Base class actions that do have a finite time duration.
 * Possible actions:
 * - An action with a duration of 0 seconds.
 * - An action with a duration of 35.5 seconds.
 * Infinite time actions are valid.
 */
class CC_DLL FiniteTimeAction : public Action{......};
```
* 跟随行为 *Follow*
```c
//cocos2d\cocos\2d\CCAction.h
/** @class Follow
 * @brief Follow is an action that "follows" a node.
 * Eg:
 * @code
 * layer->runAction(Follow::create(hero));
 * @endcode
 * Instead of using Camera as a "follower", use this action instead.
 * @since v0.99.2
 */
class CC_DLL Follow : public Action{......};
```
* 速度 *Speed*
```c
//cocos2d\cocos\2d\CCAction.h
/** @class Speed
 * @brief Changes the speed of an action, making it take longer (speed>1)
 * or shorter (speed<1) time.
 * Useful to simulate 'slow motion' or 'fast forward' effect.
 * @warning This action can't be Sequenceable because it is not an IntervalAction.
 */
class CC_DLL Speed : public Action{......};
```
* 瞬时动作 *ActionInstant*
```c
//cocos2d\cocos\2d\CCActionInstant.h
/** @class ActionInstant
* @brief Instant actions are immediate actions. They don't have a duration like the IntervalAction actions.
**/
class CC_DLL ActionInstant : public FiniteTimeAction{......};
```
* 延时动作 *ActionInterval*
```c
/** @class ActionInterval
@brief An interval action is an action that takes place within a certain period of time.
It has an start time, and a finish time. The finish time is the parameter
duration plus the start time.

These ActionInterval actions have some interesting properties, like:
- They can run normally (default)
- They can run reversed with the reverse method
- They can run with the time altered with the Accelerate, AccelDeccel and Speed actions.

For example, you can simulate a Ping Pong effect running the action normally and
then running it again in Reverse mode.

Example:

@code
auto action = MoveBy::create(1.0f, Vec2::ONE);
auto pingPongAction = Sequence::create(action, action->reverse(), nullptr);
@endcode
*/
class CC_DLL ActionInterval : public FiniteTimeAction{......};
```
