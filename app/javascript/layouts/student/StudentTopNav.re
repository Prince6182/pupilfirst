[%bs.raw {|require("./StudentTopNav.css")|}];

let str = React.string;

open StudentTopNav__Types;

let headerLink = (key, link) =>
  <div
    key
    className="md:ml-2 text-sm font-semibold text-center cursor-default flex w-1/2 sm:w-1/3 md:w-auto justify-center border-r border-b md:border-0">
    <a
      className="no-underline bg-gray-100 md:bg-white hover:bg-gray-200 text-gray-900 rounded-lg hover:text-primary-500 w-full p-4 md:px-3 md:py-2"
      href={link |> NavLink.url}
      target=?{NavLink.local(link) ? None : Some("_blank")}
      rel=?{NavLink.local(link) ? None : Some("noopener")}>
      {link |> NavLink.title |> str}
    </a>
  </div>;

let signOutLink = () =>
  <div
    key="Logout-button"
    className="md:ml-2 text-sm font-semibold cursor-default flex w-1/2 sm:w-1/3 md:w-auto justify-center border-r border-b md:border-0">
    <div className="flex items-center justify-center">
      <a
        href="/users/sign_out"
        rel="nofollow"
        className="border border-primary-500 rounded px-2 py-1 text-primary-500 text-xs md:text-sm md:leading-normal m-4 md:m-0 no-underline font-semibold">
        <FaIcon classes="fas fa-power-off" />
        <span className="ml-2"> {"Sign Out" |> str} </span>
      </a>
    </div>
  </div>;

let signInLink = () =>
  <div
    key="SignIn-button"
    className="md:ml-2 text-sm font-semibold cursor-default flex w-1/2 sm:w-1/3 md:w-auto justify-center border-r border-b md:border-0">
    <div className="flex items-center justify-center">
      <a
        className="border border-primary-500 rounded px-2 py-1 text-primary-500 text-xs md:text-sm md:leading-normal m-4 md:m-0 no-underline font-semibold"
        href="/users/sign_in">
        <FaIcon classes="fas fa-power-off" />
        <span className="ml-2"> {"Sign In" |> str} </span>
      </a>
    </div>
  </div>;
let isMobile = () => Webapi.Dom.window |> Webapi.Dom.Window.innerWidth < 768;

let headerLinks = (links, isLoggedIn, user) => {
  let (visibleLinks, dropdownLinks) =
    switch (links, isMobile()) {
    | (links, true) => (links, [])
    | ([l1, l2, l3, l4, l5, ...rest], false) => (
        [l1, l2, l3],
        [l4, l5, ...rest],
      )
    | (fourOrLessLinks, false) => (fourOrLessLinks, [])
    };

  switch (visibleLinks) {
  | visibleLinks =>
    (
      visibleLinks
      |> List.mapi((index, l) => headerLink(index |> string_of_int, l))
    )
    ->List.append([
        <StudentTopNav__DropDown links=dropdownLinks key="more-links" />,
      ])
    ->List.append([
        isLoggedIn
          ? <div className="relative flex" key="user-controls">
              <Notifications__Root
                key="notifications"
                wrapperClasses="md:ml-1 text-sm font-semibold cursor-default flex w-1/2 sm:w-1/3 md:w-auto justify-center border-r border-b md:border-0 rounded-lg hover:bg-gray-200"
                buttonClasses="font-semibold text-gray-900 hover:text-primary-500 w-full flex items-center justify-center p-4 md:px-3 md:py-2"
                icon="if i-bell-regular text-xl"
              />
              <span
                className="student-navbar__notifications-unread-bullet absolute block h-3 w-3 rounded-full border-2 border-white bg-red-500"
              />
            </div>
          : React.null,
      ])
    ->List.append([
        isLoggedIn ? <StudentTopNav__UserControls user /> : signInLink(),
      ])
    |> Array.of_list
    |> ReasonReact.array
  };
};

[@react.component]
let make = (~schoolName, ~logoUrl, ~links, ~isLoggedIn, ~currentUser) => {
  let (menuHidden, toggleMenuHidden) = React.useState(() => isMobile());

  React.useEffect(() => {
    let resizeCB = _ => toggleMenuHidden(_ => isMobile());
    Webapi.Dom.Window.asEventTarget(Webapi.Dom.window)
    |> Webapi.Dom.EventTarget.addEventListener("resize", resizeCB);
    None;
  });

  <div className="border-b">
    <div className="container mx-auto px-3 max-w-5xl">
      <nav className="flex justify-between items-center h-20">
        <div className="flex w-full items-center justify-between">
          <a className="max-w-sm" href={isLoggedIn ? "/dashboard" : "/"}>
            {switch (logoUrl) {
             | Some(url) =>
               <img
                 className="h-9 md:h-12 object-contain"
                 src=url
                 alt={"Logo of " ++ schoolName}
               />
             | None =>
               <div
                 className="p-2 rounded-lg bg-white text-gray-900 hover:bg-gray-100 hover:text-primary-600">
                 <span className="text-xl font-bold leading-tight">
                   {schoolName |> str}
                 </span>
               </div>
             }}
          </a>
          {isMobile()
             ? <div onClick={_ => toggleMenuHidden(menuHidden => !menuHidden)}>
                 <div
                   className={
                     "student-navbar__menu-btn w-8 h-8 text-center relative focus:outline-none rounded-full "
                     ++ (menuHidden ? "" : "open")
                   }>
                   <span className="student-navbar__menu-icon">
                     <span className="student-navbar__menu-icon-bar" />
                   </span>
                 </div>
               </div>
             : React.null}
        </div>
        {!menuHidden && !isMobile()
           ? <div
               className="student-navbar__links-container flex justify-end items-center w-3/5 lg:w-3/4 flex-no-wrap flex-shrink-0">
               {headerLinks(links, isLoggedIn, currentUser)}
             </div>
           : React.null}
      </nav>
    </div>
    {isMobile() && !menuHidden
       ? <div
           className="student-navbar__links-container flex flex-row border-t w-full flex-wrap shadow-lg">
           {headerLinks(links, isLoggedIn, currentUser)}
         </div>
       : React.null}
  </div>;
};
