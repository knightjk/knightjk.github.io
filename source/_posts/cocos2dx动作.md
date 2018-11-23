
---
title: cocos2dx动作
---
*《cocos2d-x游戏开发》初学笔记*

## 动作基类Action

```c
//cocos2d\cocos\2d\CCAction.h
/** 
 * @brief Base class for Action objects.
 */
class CC_DLL Action : public Ref, public Clonable{......};
```

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
```c
//cocos2d\cocos\2d\CCActionInstant.h
/** @class ActionInstant
* @brief Instant actions are immediate actions. They don't have a duration like the IntervalAction actions.
**/
class CC_DLL ActionInstant : public FiniteTimeAction{......};
```

```c
//cocos2d\cocos\2d\CCActionInstant.h
/** @class ActionInstant
* @brief Instant actions are immediate actions. They don't have a duration like the IntervalAction actions.
**/
class CC_DLL ActionInstant : public FiniteTimeAction{......};
```
