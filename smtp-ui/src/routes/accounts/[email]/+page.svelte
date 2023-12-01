<script lang="ts">
	import { readEmails } from '$lib/stores/emails';
	import { toRelativeTime } from '$lib/utils/time';
	import Icon from '@iconify/svelte';
	import type { PageData } from './$types';
	import { flip } from 'svelte/animate';
	import { slide } from 'svelte/transition';
	// import { simpleParser, type Email } from "mailparser";
	import PostalMime, { type Email } from 'postal-mime';
	import { onMount } from 'svelte';
	import { browser } from '$app/environment';
	import { bubble } from 'svelte/internal';

	export let data: PageData;

	let selected: Email | null = null;
	let emails: Email[] = [];
	let search = '';

	const id = (email: Email) => email.messageId || email.date || JSON.stringify(email);

	$: if (selected && !$readEmails.includes(id(selected)))
		$readEmails = [...$readEmails, id(selected)];

	let filtered: Email[] = [];
	$: filtered = emails.filter(
		(e) => e.subject?.includes(search) || e.text?.includes(search) || e.from?.name?.includes(search)
	);

	onMount(async () => {
		if (!browser) return;
		for (const email of data.emails) {
			let parser = new PostalMime();
			let result = await parser.parse(email);
			emails = [...emails, result];
		}
		selected = emails[0];
	});

	let mode = 'HTML';
</script>

<div class="flex items-stretch h-screen">
	<ul class="w-64">
		<li class="border-b-zinc-200 flex border-b">
			<a
				href="/accounts"
				class="border-b-zinc-200 flex items-center justify-center w-full p-4 text-3xl text-black border-b"
			>
				<Icon icon="mdi:home" />
			</a>
		</li>
		<li class="border-b-zinc-200 flex border-b">
			<input
				bind:value={search}
				type="text"
				class="grow bg-zinc-200 border-zinc-300 focus:ring-transparent focus:outline-none placeholder:text-zinc-500 shadow-zinc-200 box-border p-1 m-2 text-sm text-black border rounded-sm shadow-sm"
			/>
		</li>
		{#each filtered as email (id(email))}
			<li class="flex items-stretch" animate:flip transition:slide|local>
				<button
					class="checkox place-items-center border-b-zinc-200 grid font-bold border-b"
					on:click={() => {}}
				>
					<Icon icon="mdi:trash-can-outline" />
				</button>
				<button
					class="item"
					on:click={() => (selected = email)}
					class:font-bold={email.date && !$readEmails.includes(id(email))}
				>
					<!-- {#if email.date && !$readEmails.includes(email.date.getTime())}
						<span class="bg-zinc-700 inline-block w-2 h-2 rounded-full" />
					{/if} -->
					<div class="flex items-center justify-between">
						<span>{email.subject}</span>
						{#if email.date}
							<span>({toRelativeTime(new Date(email.date))})</span>
						{/if}
					</div>
					<span class="text-xs">{email.from?.name}</span>
				</button>
			</li>
		{/each}
	</ul>

	<div class="grow bg-zinc-200 border-zinc-300 flex flex-col">
		{#if selected}
			<div class="bg-zinc-100 shrink-0 grow-0 relative p-4 text-black">
				<div class="flex justify-between">
					<p>
						<span class="text-zinc-500">De:</span>
						{selected.from.name} ({selected.from.address})
					</p>
					<p>
						<span class="text-zinc-500">Pour:</span>
						{selected.to[0].name} ({selected.to[0].address})
					</p>
				</div>
				<p><span class="text-zinc-500">Sujet:</span> {selected.subject}</p>
				<button
					class="bg-zinc-300 right-1 bottom-1 absolute flex items-center justify-center gap-2 p-2 rounded-full"
					on:click={() => (mode = mode == 'HTML' ? 'TEXT' : 'HTML')}><Icon icon="mdi:format-text" /></button
				>
			</div>
			{#if mode == 'HTML' && selected.html}
				<main class="grow">
					<iframe
						class="border-zinc-300 w-full h-full p-2 bg-white border-2"
						title="email preview"
						srcdoc={selected.html}
						frameborder="0"
					/>
				</main>
			{:else}
				<!-- <iframe title="email preview" srcdoc={selected.text} frameborder="0" /> -->
				<pre class="border-zinc-300 w-full h-full p-2 overflow-scroll whitespace-pre-wrap bg-white border-2">
					{selected.text?.trim().replaceAll(/\n+/g, '\n\n')}
					</pre>
				<!-- else content here -->
			{/if}
		{/if}
		<!--  -->
	</div>
</div>

<style lang="postcss">
	main {
		box-shadow: inset 0 0 2px 1px #475569;
	}
	.item {
		@apply w-full p-2 border border-transparent border-b-zinc-200 text-zinc-600 cursor-pointer ease-out  transition-all;
	}
	.checkox {
		border-top: transparent;
		border-left: transparent;
		@apply grow-0 shrink-0 w-8 border-r border-r-zinc-200 text-zinc-600 cursor-pointer ease-out  transition-all;
	}
	/* .parsedMail {
		background: theme(backgroundColor.zinc.400) !important;
	} */

	.checkox:hover {
		background: theme(backgroundColor.zinc.200);
		border: 1px solid theme(borderColor.zinc.300);
	}
	.item:hover {
		background: theme(backgroundColor.zinc.200);
		border: 1px solid theme(borderColor.zinc.300);
	}
</style>
