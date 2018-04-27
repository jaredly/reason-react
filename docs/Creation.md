# Creation, Props & Self

_The documentation assumes relative familiarity with ReactJS._

ReasonReact doesn't use/need classes. The component creation API gives you a plain record, whose fields (like `render`) you can override.

The component template is created through `ReasonReact.statelessComponent("TheComponentName")`. The string being passed is for debugging purposes (the equivalent of ReactJS' [`displayName`](https://reactjs.org/docs/react-component.html#displayname)).

As an example, here's the file `Greeting.re`:

```reason;shared(sandbox);hide
[@bs.val] external sandboxDivId: string = "";
let sandboxRender = (el) => ReactDOMRe.renderToElementWithId(el, sandboxDivId);
```

```reason;no-run
let component = ReasonReact.statelessComponent("Greeting");
```

**In ReactJS**, you'd create a component class and call it through JSX which transforms into `React.createElement(myClass, {prop1: 'hello'})` under the hood. **In ReasonReact**, instead of passing the whole "class" (aka component template) into a hypothetical `ReasonReact.createElement` function, you'd instead declare a `make` function:

```reason;use(sandbox);div
# let module Greeting = {
/* still in Greeting.re */
let component = ReasonReact.statelessComponent("Greeting");

let make = (~name, _children) => {
  ...component, /* spread the template's other defaults into here  */
  render: _self => <div> {ReasonReact.stringToElement(name)} </div>
};
# };
# sandboxRender(
!# <Greeting name="Hello" />
# )
```

The `make` function is what's called by ReasonReact's JSX, described later. For now, the JSX-less way of calling & rendering a component is:

```reason;shared(Greeting);hide
let module Greeting = {
  let component = ReasonReact.statelessComponent("Greeting");

  let make = (~name, _children) => {
    ...component, /* spread the template's other defaults into here  */
    render: _self => <div> {ReasonReact.stringToElement(name)} </div>
  };
};
```

```reason;use(Greeting);use(sandbox);div
/* call the `make` function in the module `Greeting` */
let el = ReasonReact.element(Greeting.make(~name="John", [||]));
sandboxRender(el);
/* equivalent to <Greeting name="John" /> */
```

`make` asks you to return the component record created above. You'd override a few fields, such as the familiar `render`, `initialState`, `didMount`, etc., all described later.

**Note**: do **not** inline `let component` into the `make` function body like the following!

```reason;no-run
let make = _children => {
  ...(ReasonReact.statelessComponent("Greeting")),
  render: self => <div />
}
```

Since `make` is called at every JSX invocation, you'd be accidentally creating a fresh new component every time.

## Props

Props are just the labeled arguments of the `make` function, seen above. They can also be optional and/or have defaults, e.g. `let make = (~name, ~age=?, ~className="box", _children) => ...`.

The last prop **must** be `children`. If you don't use it, simply ignore it by naming it `_` or `_children`. Names starting with underscore don't trigger compiler warnings if they're unused.

**The prop name cannot be `ref` nor `key`**. Those are reserved, just like in ReactJS.

Following that example, you'd call that component in another file through `<Foo name="Reason" />`. `className`, if omitted, defaults to "box". `age` defaults to `None`. If you'd like to explicitly pass `age`, simply do so: `<Foo name="Reason" age=20 />`.

### Neat Trick with Props Forwarding

```reason;shared(Foo);hide
let module Foo = {
  let component = ReasonReact.statelessComponent("Foo");
  let make = (~name, ~age=?, children: array(ReasonReact.reactElement)) => {
    ...component,
    render: (_) => ReasonReact.stringToElement(name ++ (switch age { | None => "" | Some(age) => " is " ++ string_of_int(age)}))
  }
};
```

Sometimes in ReactJS, you're being given a prop from the owner that you'd like to forward directly to the child:

```js
<Foo name="Reason" age={this.props.age} />
```

This is a source of bugs, because `this.props.age` might be accidentally changed to a nullable number while `Foo` doesn't expect it to be so, or vice-versa; it might be nullable before, and now it's not and `Foo` is left with a useless null check somewhere in the render.

In Reason, if you want to explicitly pass an optional `ageFromProps` (whose type is `option int`, aka `None | Some int`), the following wouldn't work:

```reason;use(Foo);type-fail
let ageFromProps = Some(10);
let _ = <Foo name="Reason" age=ageFromProps />
```

Because `age` expects a normal `int` when you do call `Foo` with it, not an `option int`! Naively, you'd be forced to solve this like so:

```reason;use(Foo);use(sandbox);div
let ageFromProps = Some(10);
let el = switch (ageFromProps) {
| None => <Foo name="Reason" />
| Some(nonNullableAge) => <Foo name="Reason" age=nonNullableAge />
};
sandboxRender(el);
```

Cumbersome. Fortunately, here's a better way to explicitly pass an optional value:

```reason;use(Foo);use(sandbox);div
let ageFromProps = Some(10);
sandboxRender(<Foo name="Reason" age=?ageFromProps />)
```

It says "I understand that `age` is optional and that when I use the label I should pass an int. But I'd like to forward an `option` value explicitly". This isn't a JSX trick we've made up; it's just a language feature! See the section on "Explicitly Passed Optional" in the [Reason docs](https://reasonml.github.io/docs/en/function.html#explicitly-passed-optional).

## `self`

You might have seen the `render: (self) => ...` part in `make`. The concept of JavaScript `this` doesn't exist in ReasonReact (but can exist in Reason, since it has an optional object system); the `this` equivalent is called `self`. It's a record that contains `state`, `retainedProps`, `handle` and `reduce`, which we pass around to the lifecycle events, `render` and a few others, when they need the bag of information. These concepts will be explained later on.