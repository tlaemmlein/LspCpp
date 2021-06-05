﻿#pragma once
#include "LibLsp/lsp/method_type.h"


#include <stdexcept>
#include "LibLsp/JsonRpc/message.h"
#include "LibLsp/lsp/lsDocumentUri.h"
#include "LibLsp/lsp/lsAny.h"
#include "InitializeParams.h"




extern void Reflect(Reader&, std::pair<boost::optional<lsTextDocumentSyncKind>, boost::optional<lsTextDocumentSyncOptions> >&);

/**
 * Code Action options.
 */
struct  CodeActionOptions : WorkDoneProgressOptions {
	/**
	 * CodeActionKinds that this server may return.
	 *
	 * The list of kinds may be generic, such as `CodeActionKind.Refactor`, or the server
	 * may list out every specific kind they provide.
	 */
	typedef  std::string CodeActionKind;
	 std::vector<CodeActionKind> codeActionKinds;

	 MAKE_SWAP_METHOD(CodeActionOptions, workDoneProgress, codeActionKinds);
};
MAKE_REFLECT_STRUCT(CodeActionOptions, workDoneProgress, codeActionKinds)
struct CodeLensOptions : WorkDoneProgressOptions {
	/**
	 * Code lens has a resolve provider as well.
	 */
	boost::optional<bool> resolveProvider ;
	MAKE_SWAP_METHOD(CodeLensOptions, workDoneProgress, resolveProvider);
};
MAKE_REFLECT_STRUCT(CodeLensOptions, workDoneProgress, resolveProvider)


// Format document on type options
struct lsDocumentOnTypeFormattingOptions :WorkDoneProgressOptions {
	// A character on which formatting should be triggered, like `}`.
	std::string firstTriggerCharacter;

	// More trigger characters.
	std::vector<std::string> moreTriggerCharacter;
	MAKE_SWAP_METHOD(lsDocumentOnTypeFormattingOptions, workDoneProgress,
		firstTriggerCharacter,
		moreTriggerCharacter);
};
MAKE_REFLECT_STRUCT(lsDocumentOnTypeFormattingOptions, workDoneProgress,
	firstTriggerCharacter,
	moreTriggerCharacter);
struct RenameOptions : WorkDoneProgressOptions {
	/**
	 * Renames should be checked and tested before being executed.
	 */
	boost::optional<bool> prepareProvider;
	MAKE_SWAP_METHOD(RenameOptions, workDoneProgress, prepareProvider);
};
MAKE_REFLECT_STRUCT(RenameOptions,workDoneProgress,prepareProvider)

struct DocumentFilter{
	/**
	 * A language id, like `typescript`.
	 */
	boost::optional<std::string> language;
	/**
	 * A Uri [scheme](#Uri.scheme), like `file` or `untitled`.
	 */
	boost::optional<std::string>scheme;
	/**
	 * A glob pattern, like `*.{ts,js}`.
	 *
	 * Glob patterns can have the following syntax:
	 * - `*` to match one or more characters in a path segment
	 * - `?` to match on one character in a path segment
	 * - `**` to match any number of path segments, including none
	 * - `{}` to group sub patterns into an OR expression. (e.g. `**​/*.{ts,js}`
	 *   matches all TypeScript and JavaScript files)
	 * - `[]` to declare a range of characters to match in a path segment
	 *   (e.g., `example.[0-9]` to match on `example.0`, `example.1`, …)
	 * - `[!...]` to negate a range of characters to match in a path segment
	 *   (e.g., `example.[!0-9]` to match on `example.a`, `example.b`, but
	 *   not `example.0`)
	 */
	boost::optional<std::string>pattern;
	MAKE_SWAP_METHOD(DocumentFilter, language, scheme, pattern)
};
MAKE_REFLECT_STRUCT(DocumentFilter, language, scheme, pattern)

//A document selector is the combination of one or more document filters.
using DocumentSelector = std::vector<DocumentFilter>;

// Document link options
struct lsDocumentLinkOptions :WorkDoneProgressOptions {
	// Document links have a resolve provider as well.
	boost::optional<bool> resolveProvider;
	MAKE_SWAP_METHOD(lsDocumentLinkOptions, workDoneProgress, resolveProvider);
};
MAKE_REFLECT_STRUCT(lsDocumentLinkOptions, workDoneProgress,resolveProvider);

// Execute command options.
struct lsExecuteCommandOptions : WorkDoneProgressOptions {
	// The commands to be executed on the server
	std::vector<std::string> commands;
	MAKE_SWAP_METHOD(lsExecuteCommandOptions, workDoneProgress, commands);
};
MAKE_REFLECT_STRUCT(lsExecuteCommandOptions, workDoneProgress, commands);


struct TextDocumentRegistrationOptions
{
/**
 * A document selector to identify the scope of the registration. If set to null
 * the document selector provided on the client side will be used.
 */
	boost::optional<DocumentSelector>  documentSelector;

	MAKE_SWAP_METHOD(TextDocumentRegistrationOptions, documentSelector);
};
MAKE_REFLECT_STRUCT(TextDocumentRegistrationOptions, documentSelector);

/**
 * Static registration options to be returned in the initialize request.
 */
struct StaticRegistrationOptions :public TextDocumentRegistrationOptions
{
	/**
	 * The id used to register the request. The id can be used to deregister
	 * the request again. See also Registration#id.
	 */
	boost::optional<std::string> id;
	MAKE_SWAP_METHOD(StaticRegistrationOptions, documentSelector, id)
};
MAKE_REFLECT_STRUCT(StaticRegistrationOptions, documentSelector,id)

/**
 * The server supports workspace folder.
 *
 * Since 3.6.0
 */

struct WorkspaceFoldersOptions {
	/**
	 * The server has support for workspace folders
	 */
	boost::optional<bool>  supported;

	/**
	 * Whether the server wants to receive workspace folder
	 * change notifications.
	 *
	 * If a string is provided, the string is treated as an ID
	 * under which the notification is registered on the client
	 * side. The ID can be used to unregister for these events
	 * using the `client/unregisterCapability` request.
	 */
	boost::optional<std::pair<  boost::optional<std::string>, boost::optional<bool> > > changeNotifications;
	MAKE_SWAP_METHOD(WorkspaceFoldersOptions, supported, changeNotifications);
};
MAKE_REFLECT_STRUCT(WorkspaceFoldersOptions, supported, changeNotifications);


struct WorkspaceServerCapabilities {
	/**
	 * The server supports workspace folder.
	 *
	 * Since 3.6.0
	 */
	WorkspaceFoldersOptions workspaceFolders;
	MAKE_SWAP_METHOD(WorkspaceServerCapabilities, workspaceFolders);
};
MAKE_REFLECT_STRUCT(WorkspaceServerCapabilities, workspaceFolders);


/**
 * Semantic highlighting server capabilities.
 *
 * <p>
 * <b>Note:</b> the <a href=
 * "https://github.com/Microsoft/vscode-languageserver-node/pull/367">{@code textDocument/semanticHighlighting}
 * language feature</a> is not yet part of the official LSP specification.
 */

struct SemanticHighlightingServerCapabilities {
	/**
	 * A "lookup table" of semantic highlighting <a href="https://manual.macromates.com/en/language_grammars">TextMate scopes</a>
	 * supported by the language server. If not defined or empty, then the server does not support the semantic highlighting
	 * feature. Otherwise, clients should reuse this "lookup table" when receiving semantic highlighting notifications from
	 * the server.
	 */
	 std::vector< std::vector<std::string> > scopes;
	 MAKE_SWAP_METHOD(SemanticHighlightingServerCapabilities, scopes);
};
MAKE_REFLECT_STRUCT(SemanticHighlightingServerCapabilities, scopes);

using  DocumentColorOptions = WorkDoneProgressOptions;

struct lsServerCapabilities {
	// Defines how text documents are synced. Is either a detailed structure
	// defining each notification or for backwards compatibility the
	
	// TextDocumentSyncKind number.
	boost::optional< std::pair<boost::optional<lsTextDocumentSyncKind>,
	boost::optional<lsTextDocumentSyncOptions> >> textDocumentSync;
	
	// The server provides hover support.
	boost::optional<bool>  hoverProvider;
	
	// The server provides completion support.
	boost::optional < lsCompletionOptions > completionProvider;
	
	// The server provides signature help support.
	boost::optional < lsSignatureHelpOptions > signatureHelpProvider;
	
	// The server provides goto definition support.
	boost::optional< std::pair< boost::optional<bool>, boost::optional<WorkDoneProgressOptions> > > definitionProvider;
	

  /**
   * The server provides Goto Type Definition support.
   *
   * Since 3.6.0
   */
	boost::optional< std::pair< boost::optional<bool>, boost::optional<StaticRegistrationOptions> > > typeDefinitionProvider ;
	
	// The server provides implementation support.
	boost::optional< std::pair< boost::optional<bool>, boost::optional<StaticRegistrationOptions> > >  implementationProvider ;
	
	// The server provides find references support.
	boost::optional< std::pair< boost::optional<bool>, boost::optional<WorkDoneProgressOptions> > > referencesProvider ;
	
	// The server provides document highlight support.
	boost::optional< std::pair< boost::optional<bool>, boost::optional<WorkDoneProgressOptions> > > documentHighlightProvider ;
	
	// The server provides document symbol support.
	boost::optional< std::pair< boost::optional<bool>, boost::optional<WorkDoneProgressOptions> > > documentSymbolProvider ;
	
	// The server provides workspace symbol support.
	boost::optional< std::pair< boost::optional<bool>, boost::optional<WorkDoneProgressOptions> > > workspaceSymbolProvider ;
	
	// The server provides code actions.
	boost::optional< std::pair< boost::optional<bool>, boost::optional<CodeActionOptions> > > codeActionProvider ;
	
	// The server provides code lens.
	boost::optional<CodeLensOptions> codeLensProvider;
	
	// The server provides document formatting.
	boost::optional< std::pair< boost::optional<bool>, boost::optional<WorkDoneProgressOptions> > > documentFormattingProvider ;
	
	// The server provides document range formatting.
	boost::optional< std::pair< boost::optional<bool>, boost::optional<WorkDoneProgressOptions> > > documentRangeFormattingProvider ;
	
	// The server provides document formatting on typing.
	boost::optional<lsDocumentOnTypeFormattingOptions> documentOnTypeFormattingProvider;
	
	// The server provides rename support.
	boost::optional< std::pair< boost::optional<bool>, boost::optional<RenameOptions> > >  renameProvider;

	
	// The server provides document link support.
	boost::optional<lsDocumentLinkOptions > documentLinkProvider;


	/**
	 * The server provides color provider support.
	 *
	 * @since 3.6.0
	 */
	boost::optional< std::pair< boost::optional<bool>, boost::optional<DocumentColorOptions> > >  colorProvider;

	
	// The server provides execute command support.
	boost::optional < lsExecuteCommandOptions >executeCommandProvider;


	/**
	 * Workspace specific server capabilities
	 */
	boost::optional< WorkspaceServerCapabilities > workspace;

	/**
	 * Semantic highlighting server capabilities.
	 */

	 boost::optional<	 SemanticHighlightingServerCapabilities >semanticHighlighting;

	/**
	 * Server capability for calculating super- and subtype hierarchies.
	 * The LS supports the type hierarchy language feature, if this capability is set to {@code true}.
	 *
	 * <p>
	 * <b>Note:</b> the <a href=
	 * "https://github.com/Microsoft/vscode-languageserver-node/pull/426">{@code textDocument/typeHierarchy}
	 * language feature</a> is not yet part of the official LSP specification.
	 */
	
	 boost::optional< std::pair< boost::optional<bool>, boost::optional<StaticRegistrationOptions> > > typeHierarchyProvider;

	/**
	 * The server provides Call Hierarchy support.
	 */
	
	 boost::optional< std::pair< boost::optional<bool>, boost::optional<StaticRegistrationOptions> > > callHierarchyProvider;

	/**
	 * The server provides selection range support.
	 *
	 * Since 3.15.0
	 */
	 boost::optional< std::pair< boost::optional<bool>, boost::optional<StaticRegistrationOptions> > > selectionRangeProvider;



	boost::optional<lsp::Any> experimental;


	MAKE_SWAP_METHOD(lsServerCapabilities,
		textDocumentSync,
		hoverProvider,
		completionProvider,
		signatureHelpProvider,
		definitionProvider,
		typeDefinitionProvider,
		implementationProvider,
		referencesProvider,
		documentHighlightProvider,
		documentSymbolProvider,
		workspaceSymbolProvider,
		codeActionProvider,
		codeLensProvider,
		documentFormattingProvider,
		documentRangeFormattingProvider,
		documentOnTypeFormattingProvider,
		renameProvider,
		documentLinkProvider,
		executeCommandProvider, 
		workspace,
		semanticHighlighting,
		typeHierarchyProvider,
		callHierarchyProvider,
		selectionRangeProvider,
		experimental, colorProvider)
	
};
MAKE_REFLECT_STRUCT(lsServerCapabilities,
	textDocumentSync,
	hoverProvider,
	completionProvider,
	signatureHelpProvider,
	definitionProvider,
	typeDefinitionProvider,
	implementationProvider,
	referencesProvider,
	documentHighlightProvider,
	documentSymbolProvider,
	workspaceSymbolProvider,
	codeActionProvider,
	codeLensProvider,
	documentFormattingProvider,
	documentRangeFormattingProvider,
	documentOnTypeFormattingProvider,
	renameProvider,
	documentLinkProvider,
	executeCommandProvider,
	workspace,
	semanticHighlighting,
	typeHierarchyProvider,
	callHierarchyProvider,
	selectionRangeProvider,
	experimental, colorProvider)

