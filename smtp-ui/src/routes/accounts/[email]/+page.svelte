<script lang="ts">
	import { readEmails } from '$lib/stores/emails';
	import { toRelativeTime } from '$lib/utils/time';
	import type { PageData } from './$types';
	import type { ParsedMail } from 'mailparser';

	export let data: PageData;

	let selected: ParsedMail | null = data.emails[0];
	let selectedEmails: Set<string> = new Set();

	const id = (email: ParsedMail) =>
		email.messageId || email.date?.getTime().toString() || JSON.stringify(email);

	$: if (selected && !$readEmails.includes(id(selected)))
		$readEmails = [...$readEmails, id(selected)];

	const mode = 'HTML';
</script>

<div class="flex items-stretch h-screen">
	<ul class="w-64">
		<li class="border-b-slate-800 flex border-b">
			<button
				class="checkox border-b-transparent font-bold"
				on:click={() => {
					if (selectedEmails.size == data.emails.length) {
						selectedEmails.clear();
					} else {
						data.emails.forEach((e) => selectedEmails.add(id(e)));
					}
					selectedEmails = selectedEmails;
				}}
			>
				{#if selectedEmails.size == data.emails.length}
					+
				{/if}
			</button>
			<input
				type="text"
				class="grow bg-slate-800 border-slate-700 focus:ring-transparent focus:outline-none placeholder:text-slate-500 shadow-slate-800 box-border p-1 m-2 text-sm text-white border rounded-sm shadow-sm"
			/>
		</li>
		{#each data.emails as email}
			<!-- content here -->
			<li class="flex items-stretch">
				<button
					class="checkox border-b-slate-800 font-bold border-b"
					on:click={() => {
						selectedEmails.has(id(email))
							? selectedEmails.delete(id(email))
							: selectedEmails.add(id(email));
						selectedEmails = selectedEmails;
					}}
				>
					{#if selectedEmails.has(id(email))}
						+
					{/if}
				</button>
				<button
					class="item"
					on:click={() => (selected = email)}
					class:font-bold={email.date && !$readEmails.includes(id(email))}
				>
					<!-- {#if email.date && !$readEmails.includes(email.date.getTime())}
						<span class="bg-slate-300 inline-block w-2 h-2 rounded-full" />
					{/if} -->
					<div class="flex items-center justify-between">
						<span>{email.subject}</span>
						{#if email.date}
							<span>({toRelativeTime(email.date)})</span>
						{/if}
					</div>
					<span class="text-xs">{email.from?.text}</span>
				</button>
			</li>
		{/each}
	</ul>

	<div class="grow bg-slate-800 border-slate-700 flex flex-col">
		{#if selected}
			<div class="bg-slate-900 shrink-0 grow-0 p-4 text-white">
				<div class="flex justify-between">
					<p><span class="text-slate-500">De:</span> {selected.from?.text}</p>
					<p><span class="text-slate-500">Pour:</span> {selected.to.text || selected.to[0].text}</p>
				</div>
				<p><span class="text-slate-500">Sujet:</span> {selected.subject}</p>
			</div>
			{#if mode == 'HTML'}
				<main class="grow">
					<iframe
						class="invert w-full h-full"
						title="email preview"
						srcdoc={selected.html || selected.textAsHtml || selected.text}
						frameborder="0"
					/>
				</main>
			{:else}
				<iframe title="email preview" srcdoc={selected.text} frameborder="0" />
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
		@apply w-full p-2 border border-transparent border-b-slate-800 text-slate-400 cursor-pointer ease-out  transition-all;
	}
	.checkox {
		border-top: transparent;
		border-left: transparent;
		@apply grow-0 shrink-0 w-8 border-r border-r-slate-800 text-slate-400 cursor-pointer ease-out  transition-all;
	}
	/* .selected {
		background: theme(backgroundColor.slate.400) !important;
	} */

	.checkox:hover {
		background: theme(backgroundColor.slate.800);
		border: 1px solid theme(borderColor.slate.700);
	}
	.item:hover {
		background: theme(backgroundColor.slate.800);
		border: 1px solid theme(borderColor.slate.700);
	}
</style>
