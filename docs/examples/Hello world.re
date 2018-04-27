let str = ReasonReact.stringToElement;

let root = <div> (str("Hello playground")) </div>;

ReactDOMRe.renderToElementWithId(root, "target");
