# Conquest of SkyrimNet
 
Found your own faction in Skyrim, rule its holds, lead its armies and run its treasury, mostly by **talking to
people**. Conquest of SkyrimNet builds on Conquest of Skyrim and adds a government you speak to through
[SkyrimNet](https://github.com/MinLL/SkyrimNet-GamePlugin), along with a PrismaUI faction screen, requests from your
people, unrest and uprisings, army morale, deserters, bribery and coups, and settlements you build yourself.
 
> **New here?** Skip down to [What to say, and to whom](#what-to-say-and-to-whom). You don't need exact
> wording: say what you mean in your own words. The NPC decides whether to act, and anything that costs gold or
> can't be undone asks you to confirm first.
 
---
 
## Requirements
 
| Required | Optional |
|---|---|
| Skyrim SE/AE with SKSE | **SkyrimNet**: conversation, knowledge and memories (strongly recommended, and this guide assumes it) |
| PapyrusUtil | **Physical Letters**: requests reach you as letters by courier |
| SkyUI + MCM Helper | |
| PrismaUI | |
 
Conquest of SkyrimNet **replaces** Conquest of Skyrim (`conquerskyrim.esp`), so remove that first.
 
## Install
 
1. Install `ConquestOfSkyrimNet_v<version>.zip` with your mod manager and enable `ConquestOfSkyrimNet.esp`.
2. In the installer, keep **SkyrimNet knowledge** ticked if you use SkyrimNet.
3. Start a new game, or continue a save made with an earlier version of Conquest of SkyrimNet.
4. Read the **Start my Faction** book to pick your faction's name, army prefix and banner.
5. Go to MCM > Conquest of SkyrimNet > General and bind **Faction menu**. It's unbound by default (We recommend "8").
## Your first hour
 
1. **Found your faction** with the book "Start your faction" which is in your inventory.
2. **Take a hold.** Claim a couple areas - bandit camps, fosworn camps, giant strongholds. POSSIBLE CLAIMS: https://cdn.discordapp.com/attachments/1029345910863429662/1168911502531637338/Untitled352_20230723121708.png?ex=6b02f4ab&is=6b01a32b&hm=1095e83bc6b71c226b4f7687f7795a64d457bdfeeee797a5b67d542ef67c4d61& 
3. **Appoint a Steward.** Ask a friendly NPC: *"I need someone to run my treasury. Will you be my Steward?"*
4. **Appoint a General**, and a **Captain** for your first army.
5. **Open the faction screen** with your hotkey. It shows your treasury, supplies, lands, armies and requests.
6. From then on, run the realm by talking to the people who serve you.
---
 
## What to say, and to whom
 
The lines below are only **examples**. SkyrimNet understands what you mean, so phrase things however you like.
Who you talk to matters: each office can do certain things.
 
### Anyone (appointments)
 
| You want to… | Say something like… |
|---|---|
| Make them your **General** | "Will you be the General of my army?" |
| Make them your **Steward** | "I need a Steward to keep my treasury and trade. Will you take the post?" |
| Make them your **Blacksmith** | "Will you be my faction's Blacksmith and see to the troops' gear?" |
| Make them your **Court Wizard** | "I'd like you to serve as my Court Wizard." |
| Make them **Jarl / leader / steward / housecarl** of a town you rule | "Whiterun needs a jarl. Will you rule it for me?" · "Be Riverwood's leader." |
| Make them **Captain** of an army | "Will you lead one of my armies as Captain?" |
| **Dismiss** someone | "You're relieved of your post." |
| Have them **change clothes** (officers) | "Wear something different." · "Wear outfit template 2." |
 
Anyone holding an office may also ask to step down. You can accept or refuse.
 
### Your Steward (treasury, trade, taxes, towns)
 
| You want to… | Say something like… |
|---|---|
| Deposit gold | "Put 5,000 gold into the treasury." |
| Withdraw gold | "I need 2,000 gold from the treasury." |
| Look over the books | "Let me see the ledger." · "Show me the full economy." |
| Change a town's taxes (0–50%) | "Lower the taxes in Riverwood to 10%." · "Raise Whiterun's taxes to 30%." |
| Trade with your own towns | "Export 10 food to Riverwood." · "Import 5 wood from Whiterun." · "Stop the metal trade with Dawnstar." |
| Trade with the Imperials or Stormcloaks | "Draw up a trade with the Stormcloaks: our wood for their metal." |
| Upgrade buildings | "Upgrade the barracks in Falkreath." · "Improve the mines in Markarth." |
| Hire guards | "Hire more guards for Riverwood." |
| Declare war | "Prepare a declaration of war against the Imperials." |
| Bribe an enemy city's guards (paid from the treasury) | "Have our agents bribe the guards of Solitude." |
| Fund an enemy city's nobles | "Fund the nobles of Markarth." |
| Start a coup | "Set the coup in Whiterun in motion." |
| Found a settlement where you stand | "Let's found a settlement here." |
| Build in a settlement | "I want to build something here." |
| Calm a town in unrest | "Tell Riverwood we'll meet their demands." |
 
A town's **own jarl or steward** can do the tax, trade, upgrade and guard jobs for *their* town. Your faction's
Steward can do them for any town.
 
### Your General (armies, war, stars)
 
| You want to… | Say something like… |
|---|---|
| See every army | "Let me look over the war table." |
| Pass an order to a commander | "Tell Captain Aerina to garrison Whiterun." · "Have Army 2 return home." |
| Camp an army in a hold | "Have Army 3 make camp in the Reach." |
| Plan an attack | "Let's make war camp plans for the Pale." |
| Order an attack directly | "Send Army 1 and Army 2 to attack Fort Greymoor." |
| Begin or call off the attack | "It's time to begin the attack." · "Call off the attack." |
| Recruit troops | "Recruit 5 archers and 10 soldiers for Army 5." |
| Write a troop template | "Put 10 soldiers, 7 archers and 2 crossbowmen in template 1." |
| Apply a template | "Apply template 1 to Army 3." |
| Award a star (10,000 gold) | "Give Captain Aerina a star for her service." |
| Remove a commander | "Remove Captain Hrolf from command." |
| Name an army | "Name Army 2 the Iron Wolves, sworn to free the Reach." |
| Put down deserters | "Send the Iron Wolves against the deserters at Fort Sungard." |
| Found or build a settlement | "Found a settlement here." · "Open the build menu." |
 
### Captains and Commanders (your own army)
 
| You want to… | Say something like… |
|---|---|
| March with you | "Follow me." · "March with me." |
| Formation | "Form up." · "Break formation." |
| Gather food | "Take the army hunting." |
| Go home | "Take your army back home." |
| Garrison | "Garrison Whiterun." · "Hold Fort Greymoor." |
| Patrol or camp | "Make camp in Falkreath hold." |
| Plan or begin an attack | "Let's make war camp plans." · "Begin the attack." |
| Uniform | "Your army needs a uniform." · "Wear outfit template 3." |
| Name the army | "Call your army the Iron Wolves." |
| Award a star | "I'm awarding you a star." |
| Check morale | "How are the troops holding up?" |
| Fight deserters | "Take your army and crush those deserters." |
 
Captains and Commanders know their army's morale and what's behind it, so ask them.
 
### Your Blacksmith (gear and recruits)
 
| You want to… | Say something like… |
|---|---|
| Set troop outfits | "Let's sort out the troops' equipment." · "Give the archers their own outfit." |
| Better armour | "Can we get the troops into steel armour?" |
| Who can be recruited | "Let me choose which races can join the army." |
| Templates | "Put 8 warriors and 4 crossbowmen in template 2." · "Apply template 2 to Army 1." |
| Army uniform | "Make a uniform for Army 4." |
 
### Jarls and leaders of *other* sides (while at peace)
 
| You want to… | Say something like… |
|---|---|
| Buy their city | "What would it take for Falkreath to join my faction? I can pay." |
| Buy their hold's fort | "Sell me Fort Neugrad. Gold, food, whatever you need." |
| Buy a fort from Tullius or Ulfric | "General Tullius, I'll buy Fort Hraggstad from the Legion." |
 
### Merchants, farmers, miners and lumberjacks (supplies)
 
| You want to… | Say something like… |
|---|---|
| Buy food from farm folk | "I'll buy 30 food for my faction." |
| Buy metal from miners | "Sell me 20 ore for my army." |
| Buy wood from mill workers | "I need 40 wood. Can you spare it?" |
| Sell your surplus to people of means | "My faction has spare wood. Want to buy 50?" |
 
Farm households (such as the Battle-Borns) and people who buy wheat for favours sell **food**. Miners and mine
owners sell **metal**, and mill and lumber workers sell **wood**. Merchants sell all three.
 
### Guards, nobles and citizens of enemy cities (intrigue)
 
| You want to… | Say something like… |
|---|---|
| Bribe a guard (from your purse) | "Take this gold. Remember who your friends are when the time comes." |
| Fund a noble, merchant or citizen | "Here's gold for you and your friends. Your jarl doesn't deserve you." |
| Persuade them (free, once a day each) | "Your jarl is bleeding this city dry. You know it." |
| Launch the coup (once support is 40+) | "It's time. Help me take the jarl down." |
 
### Rebels and deserters
 
| Who | Say something like… |
|---|---|
| A **rebel leader** (or the town's jarl/steward) | "I hear you. You'll have gold, food and lower taxes. Lay down your arms." |
| A **free company** leader | "I'll pay your arrears. Swear back in." · "Keep what you hold. I'll recognise your realm." |
 
### Someone who sent you a request
 
Requests from your towns, armies and officers appear on the Requests tab, and as letters if you use Physical
Letters. Answer them on the tab or face to face:
 
- "Yes, you have my approval." (accept)
- "No, not this time." (deny)
- "Let's meet in the middle." (half the sum for half the loyalty)
### Settlers and town leaders
 
| You want to… | Say something like… |
|---|---|
| Build here | "Let's build a farmhouse." · "Open the build menu." |
| Ask how it's going | "How is the settlement doing? Are we safe from raids?" |
 
---
 
## The faction screen (hotkey)
 
The screen reopens on the last tab you used. It always has **Overview, Requests, Officers, Armies, Lands, Unrest,
Settlements, Culture** and **Settings**. Some tabs appear only when you open the screen near the right officer:
 
- **Steward** tab: near your Steward, a town steward or one of your jarls.
- **War Table** tab: near your General.
- **Blacksmith** tab: near your Blacksmith.
## Tips for new players
 
- **Speak naturally.** You don't need the exact phrases above. Name the town, army or person you mean.
- **You stay in control.** Anything with a cost or lasting effect (gold, wars, appointments, attacks, coups) shows
  a confirmation. **"Decline (silent)"** cancels it without the NPC mentioning it.
- **Watch loyalty.** Hungry, over-taxed or unguarded towns grow restless, then rise up. Unpaid, hungry or
  long-deployed armies lose morale and may desert.
- **Give stars** to good commanders. A star gives +15 morale at once and stronger troops.
- **Ask your officers.** Your Steward knows the treasury and towns, your General knows every army, and Captains
  know their own troops.
- **Check your journal.** Unrest, deserters and coups each get a quest with stages.
## Troubleshooting
 
- **The hotkey does nothing:** bind it in the MCM and found your faction first.
- **An NPC agrees but nothing happens:** make sure they hold the right office (see the tables above) and that
  SkyrimNet is running. Turn on MCM > Debug > Debug logging and check `ConquestOfSkyrimNet.log`.
- **No requests:** MCM > Requests must be on, and a town has to need something. Letters take a day to arrive.
- **No Steward / War Table tab:** stand closer to that officer (MCM > General > Officer reach).
The full in-game guide is in `Docs/ConquestOfSkyrimNet/UserGuide.md`. The changes in each version are listed in
`CHANGELOG.md`.
 
## Credits
 
Built on **Conquest of Skyrim**. Conversation through **SkyrimNet**. Menus by **PrismaUI**. Letters by
**Physical Letters**.
 
