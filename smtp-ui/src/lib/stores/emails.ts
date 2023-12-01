import { browser } from "$app/environment";
import { writable } from "svelte/store";


// Date of the emails
export const readEmails = writable<string[]>([])
readEmails.subscribe(value => {
    if(!browser) return;
    if(value.length == 0) return;

    localStorage.setItem("readEmails", JSON.stringify(value))
    console.log("saved", value)
})

if(browser) {
    console.log("loaded", JSON.parse(localStorage.getItem("readEmails") || "[]"));
    readEmails.set(JSON.parse(localStorage.getItem("readEmails") || "[]"))
}