let str = ReasonReact.stringToElement;

module CustomComponent = {
  let component = ReasonReact.statelessComponent("CustomComponent");
  let make = (~name, _children) => {
    ...component,
    render: _self => <div> (str("Hello ")) <em> (str(name)) </em> </div>,
  };
};

let root = <CustomComponent name="Folks" />;

ReactDOMRe.renderToElementWithId(root, "target");