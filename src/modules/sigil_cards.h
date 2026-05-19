#pragma once
#include <stdint.h>

struct TarotCard {
    const char* name;
    const char* symbol;   // lines separated by \n, drawn centered
    const char* suit;
    uint8_t     number;
    const char* keywords;
    const char* upright;
    const char* reversed;
};

// ── Suit symbols for Minor Arcana (6 lines each) ──────────────────────────────
static const char SYM_WAND[]  = "  .+.  \n   |   \n  \\|/  \n   |   \n  /|\\  \n  -+-  ";
static const char SYM_CUP[]   = "  ___  \n /   \\ \n| ~ ~ |\n \\___/ \n   |   \n  ---  ";
static const char SYM_SWORD[] = "   ^   \n  /|\\  \n--( )--\n   |   \n   |   \n  -v-  ";
static const char SYM_PENT[]  = "   *   \n  /*\\  \n * - * \n  \\*/  \n   *   \n  ---  ";

// ── All 78 cards ──────────────────────────────────────────────────────────────
static const TarotCard TAROT_CARDS[78] = {

// ── Major Arcana ──────────────────────────────────────────────────────────────

{ "The Fool",
  "  * . *  \n    o .  \n   /|>   \n   / \\   \n~~~~~~~~~\n  ~ ~ ~  ",
  "Major Arcana", 0,
  "Beginnings, freedom, innocence",
  "You stand at the edge of a new adventure with nothing to lose. Trust the journey and take the leap — the universe will provide a net.",
  "Reckless choices and poor judgment threaten to undermine your path. Heed the warnings around you before leaping into the unknown." },

{ "The Magician",
  "  --8--  \n    .    \n   \\o/   \n   -+-   \n  /| |\\  \n _|___|_ ",
  "Major Arcana", 1,
  "Willpower, skill, manifestation",
  "All the tools you need are already in your hands. Channel your focused willpower and take decisive action to manifest your goals now.",
  "Your talents remain untapped or are being misused. Beware of manipulation and look inward for an honest self-assessment of your abilities." },

{ "The High Priestess",
  " B     J \n |  *  | \n | /^\\ | \n | |~| | \n |     | \n |_____| ",
  "Major Arcana", 2,
  "Intuition, mystery, inner knowledge",
  "Hidden knowledge awaits beyond the veil of the conscious mind. Trust your intuition above all — the answer lives in your inner voice.",
  "You are cut off from your intuition or ignoring its signals. Secrets kept too long are surfacing, whether you are ready or not." },

{ "The Empress",
  "   ***   \n  (***) \n   _+_  \n  / | \\ \n  \\___/ \n   * *  ",
  "Major Arcana", 3,
  "Abundance, fertility, nurturing",
  "Abundance, creativity, and nurturing energy surround you. This is a fertile time for growth, beauty, and bringing your ideas into being.",
  "Creative blocks or unhealthy dependence stifle your natural growth. Reconnect with nature and your own body's wisdom to restore flow." },

{ "The Emperor",
  " /-----\\ \n|   T   |\n|  ---  |\n| (/_\\) |\n|  | |  |\n|__|_|__|",
  "Major Arcana", 4,
  "Authority, structure, stability",
  "Structure, authority, and disciplined leadership guide your path. Build lasting foundations through logic, strategy, and steadfast will.",
  "Rigid control or abuse of authority creates resistance. Examine whether you are dominating others or allowing others to dominate you." },

{ "The Hierophant",
  "    +    \n   |||   \n  /|||\\  \n   |||   \n   | |   \n __|_|__ ",
  "Major Arcana", 5,
  "Tradition, institutions, guidance",
  "Tradition, spiritual guidance, and the wisdom of established institutions offer support. Seek teachers and honor sacred conventions.",
  "Dogma and blind conformity constrain your authentic path. Question inherited beliefs and forge your own understanding of the sacred." },

{ "The Lovers",
  "   /^\\   \n  ( * )  \n   \\ /   \n  o   o  \n /|   |\\ \n *     * ",
  "Major Arcana", 6,
  "Love, harmony, choices",
  "A profound choice or deep connection stands before you. Align your values and commit with both your head and your heart fully.",
  "Misaligned values create disharmony in your relationships or deep inner conflict. Examine what you truly stand for before committing." },

{ "The Chariot",
  "  -----  \n |*   *| \n | \\o/ | \n |_____| \n  *   *  \n  o   o  ",
  "Major Arcana", 7,
  "Willpower, triumph, determination",
  "Victory comes through focused willpower and disciplined action. Control the opposing forces within and drive steadily toward your goal.",
  "Scattered energy and inner conflict derail your forward momentum. You must regain direction and discipline before pressing forward." },

{ "Strength",
  "  --8--  \n   \\o/   \n   -+-   \n  ( O )  \n  ( _ )  \n   ---   ",
  "Major Arcana", 8,
  "Courage, patience, inner power",
  "True power rises from patience, compassion, and quiet inner courage. Gentle persistence will outlast and overcome brute force.",
  "Self-doubt and fear of inadequacy drain your vital energy. Reconnect with your core strength and trust your capacity to endure." },

{ "The Hermit",
  "   .     \n    \\    \n    o    \n   /|    \n    |    \n   /\\    ",
  "Major Arcana", 9,
  "Introspection, solitude, guidance",
  "The answers you seek lie within. Withdraw from the noise of the world and follow your inner lantern into solitude and deep reflection.",
  "Isolation has become a retreat from life rather than a source of wisdom. Resist the pull toward permanent withdrawal and return." },

{ "Wheel of Fortune",
  "   . .   \n  _____  \n /T . A\\ \n|-+-+-+-|\n \\O . R/ \n  -----  ",
  "Major Arcana", 10,
  "Cycles, fate, turning points",
  "The great wheel turns and fortune shifts in your favor. Embrace cycles of change and seize the opportunities that arise in this moment.",
  "Resistance to inevitable change creates suffering. Old patterns repeat their lessons until you learn what they are meant to teach." },

{ "Justice",
  "    |    \n   / \\   \n  / . \\  \n |=====| \n    |    \n   /|\\   ",
  "Major Arcana", 11,
  "Fairness, truth, cause and effect",
  "Truth, fairness, and accountability govern this moment. Face the full consequences of past choices with honesty and clear sight.",
  "Dishonesty or unwillingness to accept responsibility distorts your judgment. Seek truth even — especially — when it is uncomfortable." },

{ "The Hanged Man",
  "  __|__  \n    |    \n    |    \n    O    \n   \\|    \n   / \\   ",
  "Major Arcana", 12,
  "Surrender, suspension, perspective",
  "Voluntary surrender opens a new way of seeing. Pause, release control, and allow the wisdom of the in-between state to emerge.",
  "Stagnation results from clinging to what no longer serves. The sacrifice required for growth is being avoided or indefinitely delayed." },

{ "Death",
  "    /    \n   / *   \n  (o o)  \n   ---   \n    |    \n   /|\\   ",
  "Major Arcana", 13,
  "Endings, transformation, transition",
  "A necessary ending clears the way for profound transformation. Release the old with grace and intention so that new life may emerge.",
  "Resistance to change keeps you locked in cycles of slow decay. The transition you fear is the very doorway you must walk through." },

{ "Temperance",
  "  [ . ]  \n   \\ /   \n    X    \n   / \\   \n  [ . ]  \n   . .   ",
  "Major Arcana", 14,
  "Balance, moderation, purpose",
  "Patience, balance, and purposeful flow bring all things into alignment. Trust the slow alchemy of time, moderation, and steady effort.",
  "Imbalance and excess disrupt the harmonious flow of your life. Reconnect with the middle path and restore what has been strained." },

{ "The Devil",
  "  /\\/\\   \n ( oo )  \n  >--<   \n  o  o   \n  |  |   \n  *  *   ",
  "Major Arcana", 15,
  "Bondage, materialism, shadow self",
  "You are confronted with chains of your own making. Acknowledge what truly binds you — only honest recognition leads to real freedom.",
  "You are reclaiming power from addiction, toxic patterns, or oppressive forces. True liberation is within reach if you act now." },

{ "The Tower",
  "   * *   \n  /---\\  \n |#####| \n |#####| \n  \\   /  \n * * * * ",
  "Major Arcana", 16,
  "Upheaval, revelation, sudden change",
  "A sudden upheaval shatters false constructs and reveals hard truths. Though painful, this collapse clears ground for something real.",
  "You sense the approaching storm and seek to delay the inevitable. Avoiding collapse now only amplifies its force when it arrives." },

{ "The Star",
  "    *    \n  * * *  \n * * * * \n    .    \n   \\o/   \n  ~~~~~  ",
  "Major Arcana", 17,
  "Hope, renewal, serenity",
  "After the storm, hope and gentle renewal shine through. Trust in the healing process and allow your spirit to be fully replenished.",
  "Despair and disconnection dim the inner light. Return to what nourishes you and remember that darkness is never the final word." },

{ "The Moon",
  "  )) ((  \n ( ~~ )  \n  )) ((  \n  |    | \n  |    | \n  ~~~~~~ ",
  "Major Arcana", 18,
  "Illusion, fear, the unconscious",
  "Illusions, fears, and unconscious forces are shaping your reality. Look beneath the surface and trust your dreams for hidden guidance.",
  "Confusion and repressed emotion cloud your perception. It is time to confront the fears you have buried in the deep unconscious." },

{ "The Sun",
  "\\ | /\n \\|/ \n-( )-\n /|\\ \n/ | \\\n  *  ",
  "Major Arcana", 19,
  "Joy, success, vitality",
  "Joy, clarity, and radiant success illuminate your path. Confidence and vital energy carry you forward into a time of pure celebration.",
  "Excessive optimism or arrogance obscures reality. Ground yourself and share the light generously rather than basking in it alone." },

{ "Judgement",
  "   )))   \n  (((    \n   \\|/   \n    o    \n   /|\\   \n  o . o  ",
  "Major Arcana", 20,
  "Reflection, reckoning, awakening",
  "A reckoning calls you to honest self-evaluation and renewal. Answer the summons with courage and be reborn through clarity and truth.",
  "Fear of judgment or refusal to examine your past keeps you stagnant. Liberation requires unflinching and compassionate self-honesty." },

{ "The World",
  " /-----\\ \n|   o   |\n|  /|\\  |\n|   |   |\n \\-----/ \n  * . *  ",
  "Major Arcana", 21,
  "Completion, integration, wholeness",
  "A great cycle reaches its magnificent completion. You have integrated all you have learned and stand in genuine wholeness and freedom.",
  "An incomplete journey leaves loose ends unresolved. Seek the final steps that will bring full closure, integration, and peace." },

// ── Wands ─────────────────────────────────────────────────────────────────────

{ "Ace of Wands", SYM_WAND, "Wands", 1,
  "Inspiration, new fire, bold start",
  "A spark of creative fire ignites a bold new beginning. Seize this moment of inspiration and let it launch you into fearless action.",
  "Creative energy is blocked or misdirected. You sense the spark within but cannot find the direction to channel your restless drive." },

{ "Two of Wands", SYM_WAND, "Wands", 2,
  "Planning, future vision, daring",
  "You hold the world in your hands and a clear vision for what comes next. Bold planning and the courage to venture beyond what is familiar.",
  "Fear of the unknown keeps you circling familiar ground. Your plans need a decisive commitment before they can become your reality." },

{ "Three of Wands", SYM_WAND, "Wands", 3,
  "Expansion, foresight, early results",
  "Your ventures are in motion and the ships are sailing. Watch your early efforts begin to expand into wider and bolder horizons.",
  "Delays and setbacks frustrate your expanding plans. Revisit your strategy and ensure your foundations are as solid as you believe." },

{ "Four of Wands", SYM_WAND, "Wands", 4,
  "Celebration, home, milestone",
  "Community, celebration, and the joy of homecoming fill this moment. Milestone achievements deserve to be honored among those you love.",
  "Tension beneath the surface disrupts the harmony of home or community. Look for what is being left unaddressed before it poisons the well." },

{ "Five of Wands", SYM_WAND, "Wands", 5,
  "Competition, conflict, friction",
  "Competitive friction and spirited conflict challenge you to sharpen your edge. Not all struggle is destructive — let it make you stronger.",
  "Pointless conflict and ego-driven battles drain energy without producing growth. Step away from fights that serve no real purpose." },

{ "Six of Wands", SYM_WAND, "Wands", 6,
  "Victory, recognition, leadership",
  "Public recognition and the sweet taste of victory reward your sustained efforts. Lead with confidence — your success is visible and deserved.",
  "Need for external validation undermines your authentic achievement. Seek inner acknowledgment rather than living for applause alone." },

{ "Seven of Wands", SYM_WAND, "Wands", 7,
  "Perseverance, defense, conviction",
  "You hold your ground against opposition and challenge. Defend your hard-won position with determination and refuse to be pushed aside.",
  "Overwhelm and self-doubt make it hard to hold your position. Assess honestly whether this stand is worth the sustained cost." },

{ "Eight of Wands", SYM_WAND, "Wands", 8,
  "Speed, momentum, swift action",
  "Events accelerate and messages fly fast. Act swiftly while the momentum carries you — hesitation will squander this rare open window.",
  "Scattered energy and crossed communications slow your momentum. Pause to clarify your direction before rushing headlong forward again." },

{ "Nine of Wands", SYM_WAND, "Wands", 9,
  "Resilience, vigilance, last stand",
  "Battle-worn but unbroken, you summon the last of your courage for the final push. Resilience and wariness now serve you well.",
  "Paranoia and exhaustion tempt you to abandon what is nearly complete. Rest and reassess, but do not surrender the whole effort." },

{ "Ten of Wands", SYM_WAND, "Wands", 10,
  "Burden, responsibility, overload",
  "A heavy burden of responsibility weighs upon your shoulders. You are carrying more than your share — delegate or release what you can.",
  "Refusing to set down burdens brings you to the point of collapse. Set aside what is not truly yours to carry and reclaim your strength." },

{ "Page of Wands", SYM_WAND, "Wands", 11,
  "Enthusiasm, exploration, daring",
  "Enthusiastic curiosity and bold energy charge forward into new territory. Embrace the adventure with complete daring and openness.",
  "Impulsive beginnings without follow-through leave projects half-finished. Focus your fire and develop the patience to see things through." },

{ "Knight of Wands", SYM_WAND, "Wands", 12,
  "Passion, impulse, adventurous",
  "Driven, charismatic, and full of raw passion, you charge toward your goal without hesitation. Channel the fire with focused wisdom.",
  "Reckless impulsiveness leaves chaos in your wake. Slow down before your unbridled enthusiasm burns everything and everyone around you." },

{ "Queen of Wands", SYM_WAND, "Wands", 13,
  "Confidence, warmth, independence",
  "Confident, creative, and warmly magnetic, you inspire everyone around you. Your passion and bold vision naturally draw success near.",
  "Jealousy or hidden self-doubt dims your natural brilliance. Reconnect with authentic fire rather than performing a version of confidence." },

{ "King of Wands", SYM_WAND, "Wands", 14,
  "Visionary, bold, inspiring leader",
  "A bold visionary leader with the courage to inspire others toward a greater future. Your creative authority and fire make the impossible real.",
  "Impulsiveness and domineering behavior alienate those who follow you. True leadership requires as much listening as it does directing." },

// ── Cups ──────────────────────────────────────────────────────────────────────

{ "Ace of Cups", SYM_CUP, "Cups", 1,
  "New love, emotional renewal, heart",
  "A wellspring of love, emotional renewal, and spiritual connection opens to you. Allow yourself to fully receive what pours forth now.",
  "Emotional blocks prevent full giving or receiving. Look within for what resists opening the heart to what is being freely offered." },

{ "Two of Cups", SYM_CUP, "Cups", 2,
  "Partnership, union, mutual respect",
  "A profound mutual connection blossoms between two souls. This bond is built on genuine understanding, deep respect, and shared feeling.",
  "Imbalance or disconnection strains a significant relationship. Honest communication is the only path to restoring the bond between you." },

{ "Three of Cups", SYM_CUP, "Cups", 3,
  "Friendship, celebration, community",
  "Friendship, celebration, and the joy of shared abundance overflow. Raise a cup with those you love and let gratitude pour freely forth.",
  "Overindulgence or social tension sours what should be joyful. Gossip and jealousy are quietly disrupting the harmony of community." },

{ "Four of Cups", SYM_CUP, "Cups", 4,
  "Apathy, contemplation, missed gifts",
  "You are withdrawn and introspective, missing gifts being offered from the outside world. Look up from brooding and open your eyes fully.",
  "Apathy is beginning to lift and new motivation stirs within you. You are growing ready to re-engage with the opportunities around you." },

{ "Five of Cups", SYM_CUP, "Cups", 5,
  "Loss, grief, regret",
  "Grief and loss weigh heavily on the spirit. Allow yourself to fully mourn what is gone, but do not forget the cups still standing behind you.",
  "You are beginning to release what was lost and return your gaze to what remains. Forgiveness and acceptance open the path forward." },

{ "Six of Cups", SYM_CUP, "Cups", 6,
  "Nostalgia, innocence, past joys",
  "Nostalgia and the warmth of happy memories bring genuine comfort. Reconnect with the simple joys that once brought you fully alive.",
  "Living in the past prevents you from fully inhabiting the present. Let cherished memories inform rather than imprison your current life." },

{ "Seven of Cups", SYM_CUP, "Cups", 7,
  "Fantasy, choices, illusion",
  "Dazzling choices and seductive fantasies distract you from taking meaningful action. Discern genuine opportunity from wishful illusion.",
  "Clarity returns after a period of confusion and wishful thinking. You are becoming ready to choose deliberately rather than dream endlessly." },

{ "Eight of Cups", SYM_CUP, "Cups", 8,
  "Abandonment, moving on, deeper meaning",
  "You are walking away from something that no longer fulfills your soul. This courageous departure leads toward something more meaningful.",
  "You linger in what no longer serves you out of fear of what lies ahead. The path forward requires releasing the comfort that remains." },

{ "Nine of Cups", SYM_CUP, "Cups", 9,
  "Contentment, wishes, satisfaction",
  "Contentment, satisfaction, and emotional abundance are fully yours. This is a time of personal fulfillment — your wishes are within reach.",
  "Material satisfaction masks a deeper and persistent emptiness. True contentment comes from within, not from the fullness of the cup alone." },

{ "Ten of Cups", SYM_CUP, "Cups", 10,
  "Harmony, lasting joy, family",
  "Lasting emotional fulfillment and harmony fill your relationships and your home. You have arrived at a place of genuine happiness.",
  "Dysfunctional patterns and unmet expectations disrupt the peace of home and family. Healing must begin with honest and open communication." },

{ "Page of Cups", SYM_CUP, "Cups", 11,
  "Sensitivity, intuition, dreams",
  "A tender and imaginative soul brings messages from the realm of deep feeling. Stay open to unexpected intuitions and creative inspiration.",
  "Emotional immaturity and wishful thinking lead to disappointing outcomes. Ground your beautiful dreams in the reality of the present." },

{ "Knight of Cups", SYM_CUP, "Cups", 12,
  "Romance, idealism, charm",
  "Romantic and idealistic, driven by deep feeling, you pursue what truly moves the heart. Let your intuition and imagination lead the way.",
  "Moodiness, jealousy, and escapist tendencies undermine your emotional connections. Bring your true feelings into honest expression." },

{ "Queen of Cups", SYM_CUP, "Cups", 13,
  "Compassion, intuition, emotional wisdom",
  "Compassionate, intuitive, and emotionally wise, you hold space for the deep currents of feeling in yourself and others. Trust your knowing.",
  "Emotional overwhelm or codependence blurs your boundary with others feelings. Reclaim your own center and restore your inner stability." },

{ "King of Cups", SYM_CUP, "Cups", 14,
  "Emotional balance, wisdom, calm",
  "Emotional maturity, wisdom, and calm authority guide your relationships. You navigate the depths of feeling with compassion and deep balance.",
  "Emotional manipulation or suppressed feeling creates turbulence beneath a composed surface. Authentic emotional expression is needed." },

// ── Swords ────────────────────────────────────────────────────────────────────

{ "Ace of Swords", SYM_SWORD, "Swords", 1,
  "Clarity, truth, decisive action",
  "A flash of mental clarity cuts through confusion and reveals the truth. Take decisive action and communicate with full honesty now.",
  "Mental fog and miscommunication create chaos around you. Seek genuine clarity before drawing conclusions or making serious accusations." },

{ "Two of Swords", SYM_SWORD, "Swords", 2,
  "Stalemate, indecision, blocked",
  "A difficult choice suspends you in careful deliberation. Gather the information you need and then trust your capacity to decide and act.",
  "Avoidance of a necessary decision only prolongs the tension. The blindfold must come off — look at the situation as it truly and fully is." },

{ "Three of Swords", SYM_SWORD, "Swords", 3,
  "Heartbreak, sorrow, loss",
  "Heartbreak and emotional pain pierce through the heart with unexpected force. Grief is real and must be honored — it carries its own wisdom.",
  "You are healing slowly from old and deep wounds. Forgiveness of yourself and others gradually and gently eases the lingering ache." },

{ "Four of Swords", SYM_SWORD, "Swords", 4,
  "Rest, retreat, recuperation",
  "Rest, recuperation, and quiet contemplation restore your full strength. Step back from the battles of the mind and allow healing silence.",
  "Restlessness and an inability to truly rest make recovery painfully slow. The mind demands to keep fighting even when the body says stop." },

{ "Five of Swords", SYM_SWORD, "Swords", 5,
  "Conflict, defeat, hollow victory",
  "A hollow victory won through aggression or cunning leaves all parties diminished. Consider whether winning this particular way is worth it.",
  "Conflict and hostility begin to subside as you choose dignity over domination. Walking away is sometimes the purest form of strength." },

{ "Six of Swords", SYM_SWORD, "Swords", 6,
  "Transition, moving on, calmer waters",
  "You are moving away from turbulence and toward calmer waters. This transition may be bittersweet, but it is fully necessary and right.",
  "You resist the journey toward healing, clinging to familiar pain and sorrow. The passage forward requires releasing what you carry with you." },

{ "Seven of Swords", SYM_SWORD, "Swords", 7,
  "Deception, strategy, cunning",
  "Cunning strategy may require operating independently. Ensure your methods remain ethical — cleverness has its own clear moral limits.",
  "Deception or self-delusion is coming to light. You may be getting away with something now, but the reckoning will inevitably arrive." },

{ "Eight of Swords", SYM_SWORD, "Swords", 8,
  "Restriction, victim, self-imposed",
  "You feel trapped and restricted, yet the bonds are largely self-imposed. The freedom you seek begins entirely within your own mind.",
  "You are beginning to recognize that your limitations have been mental constructs. Remove the blindfold and take the first step into freedom." },

{ "Nine of Swords", SYM_SWORD, "Swords", 9,
  "Anxiety, nightmares, mental anguish",
  "Anxiety and a churning mind torment you in the darkest hours. Bring your fears into the daylight where they lose their terrible power.",
  "The worst of the fears that tormented you are beginning to release their grip. Look for the dawn that follows even the longest dark night." },

{ "Ten of Swords", SYM_SWORD, "Swords", 10,
  "Painful ending, betrayal, rock bottom",
  "A painful ending or betrayal strikes its final and decisive blow. Though it hurts deeply, this is the bottom — the only direction now is up.",
  "You are surviving what felt unsurvivable. The very worst is over and you are slowly gathering the will to begin your life again." },

{ "Page of Swords", SYM_SWORD, "Swords", 11,
  "Curiosity, vigilance, sharp mind",
  "Sharp, curious, and quick-witted, approach every situation with probing questions and direct speech. Think carefully before you act.",
  "Gossip, hasty judgments, and careless words cause real and unnecessary harm. Verify before accusing and always think before you speak." },

{ "Knight of Swords", SYM_SWORD, "Swords", 12,
  "Ambition, drive, directness",
  "Driven by pure conviction and relentless momentum, you charge toward your goal. Sharpen your aim carefully and always watch your flanks.",
  "Reckless aggression and verbal brutality alienate your natural allies. Slow down significantly, listen carefully, and consider before striking." },

{ "Queen of Swords", SYM_SWORD, "Swords", 13,
  "Perceptive, direct, independent",
  "Perceptive and direct, you see clearly through all pretense. Speak your whole truth with compassion but without compromise or softening.",
  "Coldness or bitterness hides beneath sharp intelligence. Examine honestly whether past wounds have hardened into walls that isolate you." },

{ "King of Swords", SYM_SWORD, "Swords", 14,
  "Authority, truth, clear judgment",
  "Authoritative and committed to truth and justice, this mind cuts through all confusion with surgical precision. Lead with unshakeable integrity.",
  "Tyranny of intellect or abuse of authority misuses this power. Ensure your judgments are carefully balanced by wisdom and shared humanity." },

// ── Pentacles ─────────────────────────────────────────────────────────────────

{ "Ace of Pentacles", SYM_PENT, "Pentacles", 1,
  "Opportunity, prosperity, new path",
  "A doorway to material prosperity and practical opportunity opens before you. Plant this seed with genuine care and watch it slowly grow.",
  "A promising opportunity fails to manifest due to poor planning. Re-examine your foundations honestly before investing further effort." },

{ "Two of Pentacles", SYM_PENT, "Pentacles", 2,
  "Balance, adaptability, juggling",
  "You balance multiple demands with graceful adaptability. Stay light on your feet and trust your natural capacity to manage the juggle.",
  "Overwhelm and poor time management disrupt the delicate balance you maintain. Something important is quietly being neglected right now." },

{ "Three of Pentacles", SYM_PENT, "Pentacles", 3,
  "Teamwork, skill, collaboration",
  "Skilled collaboration and the pride of genuine mastery bring your work to life. Align your talents with others and build something lasting.",
  "Lack of teamwork or poor craftsmanship undermines the quality of your work. Seek honest feedback and recommit to standards of mastery." },

{ "Four of Pentacles", SYM_PENT, "Pentacles", 4,
  "Security, possessiveness, control",
  "Security and the careful conservation of resources bring genuine stability. Be mindful, however, that prudence does not harden into hoarding.",
  "Possessiveness and fear of loss create a stranglehold on what you cling to. Generosity and flexibility will ultimately serve you far better." },

{ "Five of Pentacles", SYM_PENT, "Pentacles", 5,
  "Hardship, poverty, spiritual lack",
  "Material hardship and a sense of spiritual poverty test your resilience now. Help is genuinely available if you can bring yourself to ask.",
  "The worst of this difficult period is beginning to ease and lift. Recovery comes as you accept support and make the necessary inner changes." },

{ "Six of Pentacles", SYM_PENT, "Pentacles", 6,
  "Generosity, charity, giving",
  "Generosity flows in a fair exchange of giving and receiving. Contribute freely to those in need and accept help with equal and open grace.",
  "Strings attached to generosity or a deep imbalance in giving and taking erodes trust. Examine the true motivation behind any act of charity." },

{ "Seven of Pentacles", SYM_PENT, "Pentacles", 7,
  "Patience, investment, long-term",
  "Patient tending of long-term investments yields visible signs of growth. Take stock of what has flourished and what needs renewed attention.",
  "Impatience threatens to sabotage what you have carefully built. Reassess and recommit your effort before abandoning the work mid-growth." },

{ "Eight of Pentacles", SYM_PENT, "Pentacles", 8,
  "Diligence, mastery, craftsmanship",
  "Diligent practice, focused skill-building, and honest labor define this time fully. Mastery is earned through repetition and love of craft.",
  "Shoddy work or grinding without purpose drains all meaning from your efforts. Reconnect with the deeper reason why this work truly matters." },

{ "Nine of Pentacles", SYM_PENT, "Pentacles", 9,
  "Luxury, independence, refinement",
  "Self-sufficiency and the fruits of your disciplined effort surround you beautifully. Take time to truly enjoy the elegant life you have created.",
  "Over-dependence or a superficial relationship with material comfort leaves the soul hungry. True and lasting abundance comes entirely from within." },

{ "Ten of Pentacles", SYM_PENT, "Pentacles", 10,
  "Wealth, legacy, lasting abundance",
  "Lasting wealth, family legacy, and the deep satisfaction of a life well-built are yours. What you have created will outlast your own lifetime.",
  "Family conflict or financial instability disrupts the hard-won security of your foundations. Tend honestly to what has been neglected." },

{ "Page of Pentacles", SYM_PENT, "Pentacles", 11,
  "Study, diligence, practical growth",
  "Diligent, practical, and eager to learn, this energy applies patient study and focused effort to build real-world skills. Begin the work now.",
  "Procrastination and lack of follow-through prevent genuine competence from developing. Start somewhere small and concrete — but truly start." },

{ "Knight of Pentacles", SYM_PENT, "Pentacles", 12,
  "Reliability, patience, steady effort",
  "Steadfast, reliable, and deeply committed, this knight tends to every detail with methodical care. Slow and steady genuinely wins this race.",
  "Rigidity and being stuck in a familiar rut prevent necessary adaptation. Introduce meaningful flexibility into your approach before it costs you." },

{ "Queen of Pentacles", SYM_PENT, "Pentacles", 13,
  "Nurturing, resourceful, grounded",
  "Nurturing, resourceful, and grounded in practical wisdom, she creates abundance and security for all she tends. Lead with care and earth.",
  "Neglect of the self or material insecurity undermines your capacity to nurture others. You genuinely cannot pour from an empty vessel." },

{ "King of Pentacles", SYM_PENT, "Pentacles", 14,
  "Abundance, discipline, earthly mastery",
  "Masterful in matters of wealth, business, and practical leadership, this king builds lasting abundance through discipline and earthy wisdom.",
  "Materialism, stubbornness, or corruption corrupts the stable foundations you have built. Examine honestly where greed has crept into your values." },

}; // end TAROT_CARDS
